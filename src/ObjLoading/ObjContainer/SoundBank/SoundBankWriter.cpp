#include "SoundBankWriter.h"

#include "Crypto.h"
#include "ObjContainer/SoundBank/SoundBankTypes.h"
#include "Sound/WavTypes.h"
#include "Utils/Alignment.h"
#include "Utils/FileUtils.h"

#include <filesystem>
#include <iostream>
#include <unordered_map>

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

public:
    explicit SoundBankWriterImpl::SoundBankWriterImpl(const std::string& fileName, std::ostream& stream, ISearchPath* assetSearchPath)
        : m_file_name(fileName),
          m_stream(stream),
          m_asset_search_path(assetSearchPath),
          m_sounds(),
          m_current_offset(0),
          m_total_size(0),
          m_entry_section_offset(0),
          m_checksum_section_offset(0)
    {
    }

    void AddSound(const std::string& soundFilePath, unsigned int soundId) override
    {
        this->m_sounds.push_back(std::make_pair(soundFilePath, soundId));
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
        if ((m_current_offset & 0xF) != 0)
            Pad(0x10 - (m_current_offset & 0xF));
    }

    void WriteHeader()
    {
        GoTo(0);

        SoundAssetBankChecksum checksum{};
        memset(&checksum, 0xCC, sizeof(SoundAssetBankChecksum));

        SoundAssetBankHeader header{MAGIC,
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
                                    checksum};

        strncpy(header.dependencies, m_file_name.data(), header.dependencySize);

        Write(&header, sizeof(header));
    }

    bool WriteEntries()
    {
        GoTo(DATA_OFFSET);

        for (auto& sound : m_sounds)
        {
            const auto soundFilePath = sound.first;
            const auto soundId = sound.second;

            size_t soundSize = -1;
            std::unique_ptr<char[]> soundData;

            // try to find a wav file for the sound path
            const auto wavFile = m_asset_search_path->Open(soundFilePath + ".wav");
            if (wavFile.IsOpen())
            {
                WavHeader header{};
                wavFile.m_stream->read(reinterpret_cast<char*>(&header), sizeof(WavHeader));

                soundSize = static_cast<size_t>(wavFile.m_length - sizeof(WavHeader));
                auto frameCount = soundSize / (header.formatChunk.nChannels * (header.formatChunk.wBitsPerSample / 8));
                auto frameRateIndex = INDEX_FOR_FRAMERATE[header.formatChunk.nSamplesPerSec];

                SoundAssetBankEntry entry{
                    soundId,
                    soundSize,
                    static_cast<size_t>(m_current_offset),
                    frameCount,
                    frameRateIndex,
                    static_cast<unsigned char>(header.formatChunk.nChannels),
                    0,
                    0,
                };

                m_entries.push_back(entry);

                soundData = std::make_unique<char[]>(soundSize);
                wavFile.m_stream->read(soundData.get(), soundSize);
            }
            else
            {
                // if there is no wav file, try flac file
                const auto flacFile = m_asset_search_path->Open(soundFilePath + ".wav");
                if (flacFile.IsOpen())
                {
                    soundSize = static_cast<size_t>(flacFile.m_length);

                    SoundAssetBankEntry entry{
                        soundId,
                        soundSize,
                        static_cast<size_t>(m_current_offset),
                        0,
                        0,
                        0,
                        0,
                        8,
                    };

                    m_entries.push_back(entry);

                    soundData = std::make_unique<char[]>(soundSize);
                    flacFile.m_stream->read(soundData.get(), soundSize);
                }
                else
                {
                    std::cerr << "Unable to find a compatible file for sound " << soundFilePath << std::endl;
                    return false;
                }
            }

            // calculate checksum
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

    bool Write() override
    {
        WriteEntries();
        WriteEntryList();
        WriteChecksumList();
        WriteBranding();

        m_total_size = m_current_offset;

        WriteHeader();

        if (m_current_offset > UINT32_MAX)
        {
            std::cerr << "Sound bank files must be under 4GB. Please reduce the number of sounds being written!" << std::endl;
            return false;
        }

        return true;
    }

private:
    std::string m_file_name;
    std::ostream& m_stream;
    ISearchPath* m_asset_search_path;
    std::vector<std::pair<std::string, unsigned int>> m_sounds;

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
