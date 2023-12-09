#include "SoundFileDumper.h"

#include "Csv/CsvStream.h"
#include "Dumping/AssetDumpingContext.h"
#include "Game/T6/Sound/SoundCommon.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "Sound/WavWriter.h"
#include "Utils/ClassUtils.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <unordered_set>

namespace fs = std::filesystem;

namespace T6::sound
{
    _NODISCARD std::unique_ptr<std::ostream>
        OpenAssetOutputFile(const AssetDumpingContext& context, const std::string& outputFileName, const std::string& extension)
    {
        fs::path assetPath(context.m_base_path);
        assetPath.append(GetAssetFilename(outputFileName, extension));

        auto assetDir(assetPath);
        assetDir.remove_filename();

        create_directories(assetDir);

        auto outputStream = std::make_unique<std::ofstream>(assetPath, std::ios_base::out | std::ios_base::binary);

        if (outputStream->is_open())
        {
            return std::move(outputStream);
        }

        return nullptr;
    }

    SoundBankEntryInputStream FindSoundDataInSoundBanks(const unsigned assetId)
    {
        for (const auto* soundBank : SoundBank::Repository)
        {
            auto soundFile = soundBank->GetEntryStream(assetId);
            if (soundFile.IsOpen())
                return soundFile;
        }

        return {};
    }

    void DumpSoundFilePcm(const AssetDumpingContext& context,
                          const char* assetFileName,
                          const SoundBankEntryInputStream& soundFile,
                          const unsigned bitsPerSample)
    {
        assert(soundFile.m_entry.format < SND_ASSET_FORMAT_COUNT);
        const auto outFile = OpenAssetOutputFile(context, assetFileName, EXTENSION_BY_FORMAT[soundFile.m_entry.format]);
        if (!outFile)
        {
            std::cerr << "Failed to open sound output file: \"" << assetFileName << "\"\n";
            return;
        }

        const WavWriter writer(*outFile);

        if (soundFile.m_entry.frameRateIndex >= std::extent_v<decltype(FRAME_RATE_FOR_INDEX)>)
            return;

        const WavMetaData metaData{soundFile.m_entry.channelCount, FRAME_RATE_FOR_INDEX[soundFile.m_entry.frameRateIndex], bitsPerSample};

        writer.WritePcmHeader(metaData, soundFile.m_entry.size);

        while (!soundFile.m_stream->eof())
        {
            char buffer[2048];
            soundFile.m_stream->read(buffer, sizeof(buffer));
            const auto readSize = soundFile.m_stream->gcount();
            outFile->write(buffer, readSize);
        }
    }

    void DumpSoundFilePassthrough(const AssetDumpingContext& context, const char* assetFileName, const SoundBankEntryInputStream& soundFile)
    {
        assert(soundFile.m_entry.format < SND_ASSET_FORMAT_COUNT);
        const auto outFile = OpenAssetOutputFile(context, assetFileName, EXTENSION_BY_FORMAT[soundFile.m_entry.format]);
        if (!outFile)
        {
            std::cerr << "Failed to open sound output file: \"" << assetFileName << "\"\n";
            return;
        }

        while (!soundFile.m_stream->eof())
        {
            char buffer[2048];
            soundFile.m_stream->read(buffer, sizeof(buffer));
            const auto readSize = soundFile.m_stream->gcount();
            outFile->write(buffer, readSize);
        }
    }

    void DumpSndAlias(const AssetDumpingContext& context, const SndAlias& alias)
    {
        const auto soundFile = FindSoundDataInSoundBanks(alias.assetId);
        if (soundFile.IsOpen())
        {
            const auto format = static_cast<snd_asset_format>(soundFile.m_entry.format);
            switch (format)
            {
            case SND_ASSET_FORMAT_PCMS16:
                DumpSoundFilePcm(context, alias.assetFileName, soundFile, 16u);
                break;

            case SND_ASSET_FORMAT_FLAC:
                DumpSoundFilePassthrough(context, alias.assetFileName, soundFile);
                break;

            case SND_ASSET_FORMAT_PCMS24:
            case SND_ASSET_FORMAT_PCMS32:
            case SND_ASSET_FORMAT_IEEE:
            case SND_ASSET_FORMAT_XMA4:
            case SND_ASSET_FORMAT_MSADPCM:
            case SND_ASSET_FORMAT_WMA:
            case SND_ASSET_FORMAT_WIIUADPCM:
            case SND_ASSET_FORMAT_MPC:
                std::cerr << "Cannot dump sound (Unsupported sound format " << format << "): \"" << alias.assetFileName << "\"\n";
                break;

            default:
                assert(false);
                std::cerr << "Cannot dump sound (Unknown sound format " << format << "): \"" << alias.assetFileName << "\"\n";
                break;
            }
        }
        else
        {
            std::cerr << "Could not find data for sound \"" << alias.assetFileName << "\"\n";
        }
    }

    void DumpSoundData(const AssetDumpingContext& context, const SndBank* sndBank)
    {
        std::unordered_set<unsigned> dumpedAssets;

        for (auto i = 0u; i < sndBank->aliasCount; i++)
        {
            const auto& aliasList = sndBank->alias[i];

            for (auto j = 0; j < aliasList.count; j++)
            {
                const auto& alias = aliasList.head[j];
                if (alias.assetId && alias.assetFileName && dumpedAssets.find(alias.assetId) == dumpedAssets.end())
                {
                    DumpSndAlias(context, alias);
                    dumpedAssets.emplace(alias.assetId);
                }
            }
        }
    }
}