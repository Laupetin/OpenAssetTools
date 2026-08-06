#include "SoundDumperQOS.h"

#include "Sound/WavTypes.h"
#include "Sound/WavWriter.h"
#include "Utils/Logging/Log.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <format>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace QOS;

namespace fs = std::filesystem;

namespace
{
    constexpr uint32_t WAV_CHUNK_ID_FACT = utils::MakeMagic32('f', 'a', 'c', 't');

    struct ImaAdpcmFormat
    {
        uint16_t formatTag;
        uint16_t channelCount;
        uint32_t samplesPerSecond;
        uint32_t averageBytesPerSecond;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
        uint16_t extraSize;
        uint16_t samplesPerBlock;
    };

    static_assert(sizeof(ImaAdpcmFormat) == 20);

    [[nodiscard]] fs::path GetSoundPath(const SoundFile& soundFile, const snd_alias_t& alias, const size_t aliasIndex)
    {
        fs::path result;
        if (soundFile.dir && soundFile.dir[0])
            result /= soundFile.dir;

        if (soundFile.name && soundFile.name[0])
            result /= soundFile.name;
        else if (alias.aliasName && alias.aliasName[0])
            result /= alias.aliasName;
        else
            result /= std::format("sound_{}", aliasIndex);

        if (result.is_absolute())
            result = result.relative_path();

        return result.lexically_normal();
    }

    void WriteFile(AssetDumpingContext& context, const fs::path& outputPath, const unsigned char* data, const size_t dataSize)
    {
        const auto outputFile = context.OpenAssetFile(std::format("sound/{}", outputPath.generic_string()));
        if (outputFile)
            outputFile->write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(dataSize));
    }

    void WritePayload(AssetDumpingContext& context, fs::path outputPath, const unsigned char* data, const size_t dataSize, const std::string_view extension)
    {
        outputPath.replace_extension(extension);
        WriteFile(context, outputPath, data, dataSize);
    }

    [[nodiscard]] bool ReadRange(SearchPathOpenFile& file, const uint32_t offset, const uint32_t size, std::vector<unsigned char>& data)
    {
        if (!file.IsOpen() || size == 0 || offset > static_cast<uint64_t>(file.m_length) || size > static_cast<uint64_t>(file.m_length) - offset
            || !std::in_range<std::streamsize>(size))
        {
            return false;
        }

        file.m_stream->seekg(offset, std::ios::beg);
        if (!*file.m_stream)
            return false;

        data.resize(size);
        file.m_stream->read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size));
        return file.m_stream->gcount() == static_cast<std::streamsize>(size);
    }

    [[nodiscard]] bool ReadFile(SearchPathOpenFile& file, std::vector<unsigned char>& data)
    {
        if (!file.IsOpen() || file.m_length <= 0 || static_cast<uint64_t>(file.m_length) > std::numeric_limits<uint32_t>::max())
            return false;

        return ReadRange(file, 0, static_cast<uint32_t>(file.m_length), data);
    }

    [[nodiscard]] bool DumpPackedSound(AssetDumpingContext& context, const fs::path& outputPath, const SoundFile& soundFile)
    {
        // The game addresses streams as "*main/bxpack%03d.bin*<size>*<offset>.mp3"; packed streamed sounds are always MP3.
        for (size_t languageIndex = 0; languageIndex < std::size(soundFile.packageIndex); languageIndex++)
        {
            const auto packageIndex = soundFile.packageIndex[languageIndex];
            if (packageIndex == SOUND_PACKAGE_INDEX_LOOSE || soundFile.packageSize[languageIndex] == 0)
                continue;

            auto package = context.m_obj_search_path.Open(std::format("bxpack{:03}.bin", packageIndex));
            std::vector<unsigned char> data;
            if (!ReadRange(package, soundFile.packageOffset[languageIndex], soundFile.packageSize[languageIndex], data))
                continue;

            WritePayload(context, outputPath, data.data(), data.size(), ".mp3");
            return true;
        }

        return false;
    }

    [[nodiscard]] bool DumpLooseSound(AssetDumpingContext& context, const fs::path& outputPath)
    {
        auto wavPath = outputPath;
        wavPath.replace_extension(".wav");
        auto mp3Path = outputPath;
        mp3Path.replace_extension(".mp3");

        const std::array candidates{outputPath, wavPath, mp3Path};
        for (size_t candidateIndex = 0; candidateIndex < candidates.size(); candidateIndex++)
        {
            const auto& candidate = candidates[candidateIndex];
            if ((candidateIndex > 0 && candidate == candidates[0]) || (candidateIndex > 1 && candidate == candidates[1]))
                continue;

            auto inputFile = context.m_obj_search_path.Open(std::format("sound/{}", candidate.generic_string()));
            std::vector<unsigned char> data;
            if (!ReadFile(inputFile, data))
                continue;

            WriteFile(context, candidate, data.data(), data.size());
            return true;
        }

        return false;
    }

    [[nodiscard]] bool DumpStreamedSound(AssetDumpingContext& context, const fs::path& outputPath, const SoundFile& soundFile)
    {
        return DumpPackedSound(context, outputPath, soundFile) || DumpLooseSound(context, outputPath);
    }

    [[nodiscard]] bool DumpPcm(AssetDumpingContext& context, fs::path outputPath, const MssSound& sound)
    {
        const auto& info = sound.info;
        if (info.channels <= 0 || info.rate == 0 || info.bits <= 0)
            return false;

        // The asset name can end in .mp3, but this format value identifies raw PCM data that must be wrapped in a WAV container.
        outputPath.replace_extension(".wav");
        const auto outputFile = context.OpenAssetFile(std::format("sound/{}", outputPath.generic_string()));
        if (!outputFile)
            return true;

        const WavMetaData metaData{
            .channelCount = static_cast<unsigned>(info.channels),
            .samplesPerSec = info.rate,
            .bitsPerSample = static_cast<unsigned>(info.bits),
        };
        const WavWriter writer(*outputFile);
        writer.WritePcmHeader(metaData, info.data_len);
        writer.WritePcmData(sound.data, info.data_len);
        return true;
    }

    [[nodiscard]] bool DumpImaAdpcm(AssetDumpingContext& context, fs::path outputPath, const MssSound& sound)
    {
        const auto& info = sound.info;
        if (info.channels <= 0 || info.channels > std::numeric_limits<uint16_t>::max() || info.rate == 0 || info.block_size == 0
            || info.block_size > std::numeric_limits<uint16_t>::max())
        {
            return false;
        }

        const auto channelCount = static_cast<uint32_t>(info.channels);
        if (info.block_size <= 4 * channelCount)
            return false;

        const auto samplesPerBlock = 1u + 2u * (info.block_size - 4u * channelCount) / channelCount;
        if (samplesPerBlock == 0 || samplesPerBlock > std::numeric_limits<uint16_t>::max())
            return false;

        // The asset name can end in .mp3, but this format value identifies raw IMA ADPCM data that must be wrapped in a WAV container.
        outputPath.replace_extension(".wav");
        const auto outputFile = context.OpenAssetFile(std::format("sound/{}", outputPath.generic_string()));
        if (!outputFile)
            return true;

        const auto paddedDataSize = info.data_len + (info.data_len & 1u);
        const auto riffSize = static_cast<uint32_t>(sizeof(WAV_WAVE_ID) + sizeof(WavChunkHeader) + sizeof(ImaAdpcmFormat) + sizeof(WavChunkHeader)
                                                    + sizeof(uint32_t) + sizeof(WavChunkHeader) + paddedDataSize);
        const WavChunkHeader formatHeader{.chunkID = WAV_CHUNK_ID_FMT, .chunkSize = sizeof(ImaAdpcmFormat)};
        const WavChunkHeader factHeader{.chunkID = WAV_CHUNK_ID_FACT, .chunkSize = sizeof(uint32_t)};
        const WavChunkHeader dataHeader{.chunkID = WAV_CHUNK_ID_DATA, .chunkSize = info.data_len};
        const ImaAdpcmFormat format{
            .formatTag = std::to_underlying(QOS_SOUND_FORMAT_IMA_ADPCM),
            .channelCount = static_cast<uint16_t>(channelCount),
            .samplesPerSecond = info.rate,
            .averageBytesPerSecond = static_cast<uint32_t>(static_cast<uint64_t>(info.rate) * info.block_size / samplesPerBlock),
            .blockAlign = static_cast<uint16_t>(info.block_size),
            .bitsPerSample = 4,
            .extraSize = sizeof(uint16_t),
            .samplesPerBlock = static_cast<uint16_t>(samplesPerBlock),
        };

        outputFile->write(reinterpret_cast<const char*>(&WAV_CHUNK_ID_RIFF), sizeof(WAV_CHUNK_ID_RIFF));
        outputFile->write(reinterpret_cast<const char*>(&riffSize), sizeof(riffSize));
        outputFile->write(reinterpret_cast<const char*>(&WAV_WAVE_ID), sizeof(WAV_WAVE_ID));
        outputFile->write(reinterpret_cast<const char*>(&formatHeader), sizeof(formatHeader));
        outputFile->write(reinterpret_cast<const char*>(&format), sizeof(format));
        outputFile->write(reinterpret_cast<const char*>(&factHeader), sizeof(factHeader));
        outputFile->write(reinterpret_cast<const char*>(&info.samples), sizeof(info.samples));
        outputFile->write(reinterpret_cast<const char*>(&dataHeader), sizeof(dataHeader));
        outputFile->write(reinterpret_cast<const char*>(sound.data), static_cast<std::streamsize>(info.data_len));
        if (info.data_len & 1u)
            outputFile->put('\0');

        return true;
    }

    void DumpMssSound(AssetDumpingContext& context, const fs::path& outputPath, const MssSound& sound, const char* aliasName)
    {
        const auto& info = sound.info;
        if (info.data_len == 0)
            return;

        switch (info.format)
        {
        case QOS_SOUND_FORMAT_PCM:
            if (DumpPcm(context, outputPath, sound))
                return;
            break;

        case QOS_SOUND_FORMAT_IMA_ADPCM:
            if (DumpImaAdpcm(context, outputPath, sound))
                return;
            break;
        }

        WritePayload(context, outputPath, sound.data, info.data_len, ".bin");
        con::warn("Dumped QOS sound '{}' as raw data because MSS format {} is not supported", aliasName ? aliasName : "", std::to_underlying(info.format));
    }

    void DumpPrimedSound(AssetDumpingContext& context, const fs::path& outputPath, const SoundFile& soundFile, const char* aliasName)
    {
        if (DumpStreamedSound(context, outputPath, soundFile))
            return;

        const auto* sound = soundFile.u.primeSnd;
        if (!sound || !sound->buffer || sound->size == 0)
            return;

        const auto* data = reinterpret_cast<const unsigned char*>(sound->buffer);
        WritePayload(context, outputPath, data, sound->size, ".bin");
        con::warn("Could not find packed QOS sound '{}'; dumped its incomplete priming buffer as raw data", aliasName ? aliasName : "");
    }
} // namespace

namespace sound
{
    void DumperQOS::DumpAsset(AssetDumpingContext& context, const XAssetInfo<snd_alias_list_t>& asset)
    {
        const auto* aliasList = asset.Asset();
        if (!aliasList->head || aliasList->count <= 0)
            return;

        std::unordered_set<const SoundFile*> dumpedSoundFiles;
        for (size_t aliasIndex = 0; aliasIndex < static_cast<size_t>(aliasList->count); aliasIndex++)
        {
            const auto& alias = aliasList->head[aliasIndex];
            const auto* soundFile = alias.soundFile;
            if (!soundFile || !dumpedSoundFiles.emplace(soundFile).second)
                continue;

            const auto outputPath = GetSoundPath(*soundFile, alias, aliasIndex);
            switch (soundFile->type)
            {
            case SAT_LOADED:
                if (soundFile->u.mssSound)
                    DumpMssSound(context, outputPath, *soundFile->u.mssSound, alias.aliasName);
                break;

            case SAT_STREAMED:
                (void)DumpStreamedSound(context, outputPath, *soundFile);
                break;

            case SAT_PRIMED:
                DumpPrimedSound(context, outputPath, *soundFile, alias.aliasName);
                break;

            default:
                break;
            }
        }
    }
} // namespace sound
