#include "SoundBankWriter.h"

#include "Crypto.h"
#include "ObjContainer/SoundBank/SoundBankTypes.h"
#include "Sound/FlacDecoder.h"
#include "Sound/WavTypes.h"
#include "Utils/FileUtils.h"
#include "Utils/StringUtils.h"

#include <cstring>
#include <filesystem>
#include <format>
#include <iostream>
#include <unordered_map>

namespace fs = std::filesystem;

std::unordered_map<unsigned int, unsigned char> INDEX_FOR_FRAMERATE{
    {8000,   0},
    {12000,  1},
    {16000,  2},
    {24000,  3},
    {32000,  4},
    {44100,  5},
    {48000,  6},
    {96000,  7},
    {192000, 8},
};

class SoundBankWriterImpl : public SoundBankWriter
{
    static constexpr char BRANDING[] = "Created with OAT - OpenAssetTools";
    static constexpr int64_t DATA_OFFSET = 0x800;
    static constexpr uint32_t MAGIC = FileUtils::MakeMagic32('2', 'U', 'X', '#');
    static constexpr uint32_t VERSION = 14u;

    inline static const std::string PAD_DATA = std::string(16, '\x00');

    class SoundBankEntryInfo
    {
    public:
        SoundBankEntryInfo()
            : m_sound_id(0u),
              m_looping(false),
              m_streamed(false)
        {
        }

        SoundBankEntryInfo(std::string filePath, const unsigned int soundId, const bool looping, const bool streamed)
            : m_file_path(std::move(filePath)),
              m_sound_id(soundId),
              m_looping(looping),
              m_streamed(streamed)
        {
        }

        std::string m_file_path;
        unsigned int m_sound_id;
        bool m_looping;
        bool m_streamed;
    };

public:
    explicit SoundBankWriterImpl(std::string fileName, std::ostream& stream, ISearchPath* assetSearchPath)
        : m_file_name(std::move(fileName)),
          m_stream(stream),
          m_asset_search_path(assetSearchPath),
          m_current_offset(0),
          m_total_size(0),
          m_entry_section_offset(0),
          m_checksum_section_offset(0)
    {
    }

    void AddSound(const std::string& soundFilePath, unsigned int soundId, bool looping, bool streamed) override
    {
        auto itr = std::ranges::find_if(this->m_sounds,
                                        [soundId](SoundBankEntryInfo& entry)
                                        {
                                            return entry.m_sound_id == soundId;
                                        });

        if (itr == this->m_sounds.end())
        {
            this->m_sounds.emplace_back(soundFilePath, soundId, looping, streamed);
        }
    }

    void GoTo(const int64_t offset)
    {
        m_stream.seekp(offset, std::ios::beg);
        m_current_offset = offset;
    }

    void Write(const void* data, const size_t dataSize)
    {
        m_stream.write(static_cast<const char*>(data), dataSize);
        m_current_offset += dataSize;
    }

    void Pad(const size_t paddingSize)
    {
        auto paddingSizeLeft = paddingSize;
        while (paddingSizeLeft > 0)
        {
            const auto writeSize = std::min(paddingSizeLeft, PAD_DATA.size());
            Write(PAD_DATA.data(), writeSize);

            paddingSizeLeft -= writeSize;
        }
    }

    void AlignToChunk()
    {
        if (m_current_offset % 16 != 0)
            Pad(16 - (m_current_offset % 16));
    }

    void WriteHeader()
    {
        GoTo(0);

        // The checksum here is supposed to be a MD5 of the entire data portion of the file. However T6 does not validate this.
        // As long as the values here match the values in the SndBank asset, everything loads fine
        SoundAssetBankChecksum checksum{};
        memset(&checksum, 0xCC, sizeof(SoundAssetBankChecksum));

        SoundAssetBankHeader header{
            MAGIC,
            VERSION,
            sizeof(SoundAssetBankEntry),
            sizeof(SoundAssetBankChecksum),
            0x40,
            m_entries.size(),
            0,
            0,
            m_total_size,
            m_entry_section_offset,
            m_checksum_section_offset,
            checksum,
            {},
        };

        strncpy(header.dependencies, m_file_name.data(), header.dependencySize);

        Write(&header, sizeof(header));
    }

    bool LoadWavFile(const SearchPathOpenFile& file, const SoundBankEntryInfo& sound, std::unique_ptr<char[]>& soundData, size_t& soundSize)
    {
        WavHeader header{};
        file.m_stream->read(reinterpret_cast<char*>(&header), sizeof(WavHeader));

        soundSize = static_cast<size_t>(file.m_length - sizeof(WavHeader));
        const auto frameCount = soundSize / (header.formatChunk.nChannels * (header.formatChunk.wBitsPerSample / 8));
        const auto frameRateIndex = INDEX_FOR_FRAMERATE[header.formatChunk.nSamplesPerSec];

        SoundAssetBankEntry entry{
            sound.m_sound_id,
            soundSize,
            static_cast<size_t>(m_current_offset),
            frameCount,
            frameRateIndex,
            static_cast<unsigned char>(header.formatChunk.nChannels),
            sound.m_looping,
            0,
        };

        m_entries.push_back(entry);

        soundData = std::make_unique<char[]>(soundSize);
        file.m_stream->read(soundData.get(), soundSize);

        return true;
    }

    bool LoadFlacFile(
        const SearchPathOpenFile& file, const std::string& filePath, const SoundBankEntryInfo& sound, std::unique_ptr<char[]>& soundData, size_t& soundSize)
    {
        soundSize = static_cast<size_t>(file.m_length);

        soundData = std::make_unique<char[]>(soundSize);
        file.m_stream->read(soundData.get(), soundSize);

        flac::FlacMetaData metaData;
        if (flac::GetFlacMetaData(soundData.get(), soundSize, metaData))
        {
            const auto frameRateIndex = INDEX_FOR_FRAMERATE[metaData.m_sample_rate];
            SoundAssetBankEntry entry{
                sound.m_sound_id,
                soundSize,
                static_cast<size_t>(m_current_offset),
                static_cast<unsigned>(metaData.m_total_samples),
                frameRateIndex,
                metaData.m_number_of_channels,
                sound.m_looping,
                8,
            };

            m_entries.push_back(entry);
            return true;
        }

        std::cerr << std::format("Unable to decode .flac file for sound {}\n", filePath);
        return false;
    }

    bool LoadFileByExtension(const std::string& filePath, const SoundBankEntryInfo& sound, std::unique_ptr<char[]>& soundData, size_t& soundSize)
    {
        auto extension = fs::path(filePath).extension().string();
        utils::MakeStringLowerCase(extension);
        if (extension.empty())
            return false;

        const auto file = m_asset_search_path->Open(filePath);
        if (!file.IsOpen())
            return false;

        if (extension == ".wav")
            return LoadWavFile(file, sound, soundData, soundSize);

        if (extension == ".flac")
            return LoadFlacFile(file, filePath, sound, soundData, soundSize);

        return false;
    }

    bool GuessFilenameAndLoadFile(const std::string& filePath, const SoundBankEntryInfo& sound, std::unique_ptr<char[]>& soundData, size_t& soundSize)
    {
        fs::path pathWithExtension = fs::path(filePath).replace_extension(".wav");
        auto file = m_asset_search_path->Open(pathWithExtension.string());
        if (file.IsOpen())
            return LoadWavFile(file, sound, soundData, soundSize);

        pathWithExtension = fs::path(filePath).replace_extension(".flac");
        file = m_asset_search_path->Open(pathWithExtension.string());
        if (file.IsOpen())
            return LoadFlacFile(file, pathWithExtension.string(), sound, soundData, soundSize);

        return false;
    }

    bool WriteEntries()
    {
        GoTo(DATA_OFFSET);

        for (auto& sound : m_sounds)
        {
            const auto& soundFilePath = sound.m_file_path;

            size_t soundSize;
            std::unique_ptr<char[]> soundData;

            if (!LoadFileByExtension(soundFilePath, sound, soundData, soundSize) && !GuessFilenameAndLoadFile(soundFilePath, sound, soundData, soundSize))
            {
                std::cerr << std::format("Unable to find a compatible file for sound {}\n", soundFilePath);
                return false;
            }

            const auto lastEntry = m_entries.rbegin();
            if (!sound.m_streamed && lastEntry->frameRateIndex != 6)
            {
                std::cout << std::format("WARNING: Loaded sound \"{}\" should have a framerate of 48000 but doesn't. This sound may not work on all games!\n",
                                         soundFilePath);
            }

            SoundAssetBankChecksum checksum{};

            const auto md5Crypt = Crypto::CreateMD5();
            md5Crypt->Process(soundData.get(), soundSize);
            md5Crypt->Finish(checksum.checksumBytes);

            m_checksums.push_back(checksum);

            // write data
            Write(soundData.get(), soundSize);
        }

        return true;
    }

    void WriteEntryList()
    {
        AlignToChunk();

        m_entry_section_offset = m_current_offset;

        for (auto& entry : m_entries)
        {
            Write(&entry, sizeof(SoundAssetBankEntry));
        }
    }

    void WriteChecksumList()
    {
        m_checksum_section_offset = m_current_offset;

        for (auto& checksum : m_checksums)
        {
            Write(&checksum, sizeof(SoundAssetBankChecksum));
        }
    }

    void WriteBranding()
    {
        AlignToChunk();
        Write(BRANDING, sizeof(BRANDING));
        AlignToChunk();
    }

    bool Write(size_t& dataSize) override
    {
        if (!WriteEntries())
        {
            std::cerr << "An error occurred writing the sound bank entries. Please check output.\n";
            return false;
        }

        WriteEntryList();
        WriteChecksumList();
        WriteBranding();

        m_total_size = m_current_offset;

        WriteHeader();

        if (m_current_offset > UINT32_MAX)
        {
            std::cerr << "Sound bank files must be under 4GB. Please reduce the number of sounds being written!\n";
            return false;
        }

        // output the total size for the sound asset data
        dataSize = static_cast<size_t>(m_entry_section_offset - DATA_OFFSET);
        return true;
    }

private:
    std::string m_file_name;
    std::ostream& m_stream;
    ISearchPath* m_asset_search_path;
    std::vector<SoundBankEntryInfo> m_sounds;

    int64_t m_current_offset;
    std::vector<SoundAssetBankEntry> m_entries;
    std::vector<SoundAssetBankChecksum> m_checksums;
    int64_t m_total_size;
    int64_t m_entry_section_offset;
    int64_t m_checksum_section_offset;
};

std::filesystem::path SoundBankWriter::OutputPath;

std::unique_ptr<SoundBankWriter> SoundBankWriter::Create(const std::string& fileName, std::ostream& stream, ISearchPath* assetSearchPath)
{
    return std::make_unique<SoundBankWriterImpl>(fileName, stream, assetSearchPath);
}
