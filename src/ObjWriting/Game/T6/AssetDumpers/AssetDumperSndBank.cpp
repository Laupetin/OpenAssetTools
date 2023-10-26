#include "AssetDumperSndBank.h"

#include <fstream>
#include <filesystem>
#include <unordered_set>

#include "Utils/ClassUtils.h"
#include "Csv/CsvStream.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "Sound/WavWriter.h"

using namespace T6;
namespace fs = std::filesystem;

namespace
{
    const std::string ALIAS_HEADERS[]
    {
        "# name",
        "# file",
        "# template",
        "# loadspec",
        "# secondary",
        "# group",
        "# vol_min",
        "# vol_max",
        "# team_vol_mod",
        "# dist_min",
        "# dist_max",
        "# dist_reverb_max",
        "# volume_falloff_curve",
        "# reverb_falloff_curve",
        "# volume_min_falloff_curve",
        "# reverb_min_falloff_curve",
        "# limit_count",
        "# limit_type",
        "# entity_limit_count",
        "# entity_limit_type",
        "# pitch_min",
        "# pitch_max",
        "# team_pitch_mod",
        "# min_priority",
        "# max_priority",
        "# min_priority_threshold",
        "# max_priority_threshold",
        "# spatialized",
        "# type",
        "# loop",
        "# randomize_type",
        "# probability",
        "# start_delay",
        "# reverb_send",
        "# duck",
        "# pan",
        "# center_send",
        "# envelop_min",
        "# envelop_max",
        "# envelop_percentage",
        "# occlusion_level",
        "# occlusion_wet_dry",
        "# is_big",
        "# distance_lpf",
        "# move_type",
        "# move_time",
        "# real_delay",
        "# subtitle",
        "# mature",
        "# doppler",
        "# futz",
        "# context_type",
        "# context_value",
        "# compression",
        "# timescale",
        "# music",
        "# fade_in",
        "# fade_out",
        "# pc_format",
        "# pause",
        "# stop_on_death",
        "# bus",
        "# snapshot",
    };

    const std::string PREFIXES_TO_DROP[]
    {
        "raw/",
        "devraw/",
    };

    constexpr size_t FRAME_RATE_FOR_INDEX[]
    {
        8000,
        12000,
        16000,
        24000,
        32000,
        44100,
        48000,
        96000,
        192000
    };
}

class AssetDumperSndBank::Internal
{
    AssetDumpingContext& m_context;

    _NODISCARD std::string GetAssetFilename(std::string outputFileName, const std::string& extension) const
    {
        fs::path assetPath(m_context.m_base_path);

        std::replace(outputFileName.begin(), outputFileName.end(), '\\', '/');
        for (const auto& droppedPrefix : PREFIXES_TO_DROP)
        {
            if (outputFileName.rfind(droppedPrefix, 0) != std::string::npos)
            {
                outputFileName.erase(0, droppedPrefix.size());
                break;
            }
        }

        assetPath.append(outputFileName);
        if (!extension.empty())
            assetPath.concat(extension);

        return assetPath.string();
    }

    _NODISCARD std::unique_ptr<std::ostream> OpenAssetOutputFile(const std::string& outputFileName, const std::string& extension) const
    {
        fs::path assetPath(GetAssetFilename(outputFileName, extension));

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

    static std::unique_ptr<std::ostream> OpenAliasOutputFile(const SndBank* sndBank)
    {
        return nullptr;
    }

    static void WriteAliasFileHeader(CsvOutputStream& stream)
    {
        for (const auto& headerField : ALIAS_HEADERS)
        {
            stream.WriteColumn(headerField);
        }

        stream.NextRow();
    }

    static void WriteAliasToFile(CsvOutputStream& stream, const SndAlias* alias)
    {
        // name
        stream.WriteColumn(alias->name);

        // file
        stream.WriteColumn(alias->assetFileName);

        // "# template",
        // template
        stream.WriteColumn("");

        // loadspec
        stream.WriteColumn("");

        //     "# secondary",
        //     "# group",
        //     "# vol_min",
        //     "# vol_max",
        //     "# team_vol_mod",
        //     "# dist_min",
        //     "# dist_max",
        //     "# dist_reverb_max",
        //     "# volume_falloff_curve",
        //     "# reverb_falloff_curve",
        //     "# volume_min_falloff_curve",
        //     "# reverb_min_falloff_curve",
        //     "# limit_count",
        //     "# limit_type",
        //     "# entity_limit_count",
        //     "# entity_limit_type",
        //     "# pitch_min",
        //     "# pitch_max",
        //     "# team_pitch_mod",
        //     "# min_priority",
        //     "# max_priority",
        //     "# min_priority_threshold",
        //     "# max_priority_threshold",
        //     "# spatialized",
        //     "# type",
        //     "# loop",
        //     "# randomize_type",
        //     "# probability",
        //     "# start_delay",
        //     "# reverb_send",
        //     "# duck",
        //     "# pan",
        //     "# center_send",
        //     "# envelop_min",
        //     "# envelop_max",
        //     "# envelop_percentage",
        //     "# occlusion_level",
        //     "# occlusion_wet_dry",
        //     "# is_big",
        //     "# distance_lpf",
        //     "# move_type",
        //     "# move_time",
        //     "# real_delay",
        //     "# subtitle",
        //     "# mature",
        //     "# doppler",
        //     "# futz",
        //     "# context_type",
        //     "# context_value",
        //     "# compression",
        //     "# timescale",
        //     "# music",
        //     "# fade_in",
        //     "# fade_out",
        //     "# pc_format",
        //     "# pause",
        //     "# stop_on_death",
        //     "# bus",
        //     "# snapshot",
    }

    static void DumpSndBankAliases(const SndBank* sndBank)
    {
        const auto outputFile = OpenAliasOutputFile(sndBank);

        if (outputFile == nullptr)
        {
            std::cout << "Failed to open sound alias output file for: \"" << sndBank->name << "\"" << std::endl;
            return;
        }

        CsvOutputStream csvStream(*outputFile);
        WriteAliasFileHeader(csvStream);

        for (auto i = 0u; i < sndBank->aliasCount; i++)
        {
            const auto& aliasList = sndBank->alias[i];
            for (auto j = 0; j < aliasList.count; j++)
            {
                const auto& alias = aliasList.head[j];
                WriteAliasToFile(csvStream, &alias);
            }
        }
    }

    static SoundBankEntryInputStream FindSoundDataInSoundBanks(const unsigned assetId)
    {
        for (const auto* soundBank : SoundBank::Repository)
        {
            auto soundFile = soundBank->GetEntryStream(assetId);
            if (soundFile.IsOpen())
                return soundFile;
        }

        return {};
    }

    void DumpSoundFilePcm(const char* assetFileName, const SoundBankEntryInputStream& soundFile, const unsigned bitsPerSample) const
    {
        const auto outFile = OpenAssetOutputFile(assetFileName, ".wav");
        if (!outFile)
        {
            std::cerr << "Failed to open sound output file: \"" << assetFileName << "\"\n";
            return;
        }

        const WavWriter writer(*outFile);

        if (soundFile.m_entry.frameRateIndex >= std::extent_v<decltype(FRAME_RATE_FOR_INDEX)>)
            return;

        const WavMetaData metaData{
            soundFile.m_entry.channelCount,
            FRAME_RATE_FOR_INDEX[soundFile.m_entry.frameRateIndex],
            bitsPerSample
        };

        writer.WritePcmHeader(metaData, soundFile.m_entry.size);

        while (!soundFile.m_stream->eof())
        {
            char buffer[2048];
            soundFile.m_stream->read(buffer, sizeof(buffer));
            const auto readSize = soundFile.m_stream->gcount();
            outFile->write(buffer, readSize);
        }
    }

    void DumpSoundFilePassthrough(const char* assetFileName, const SoundBankEntryInputStream& soundFile, const std::string& extension) const
    {
        const auto outFile = OpenAssetOutputFile(assetFileName, extension);
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

    void DumpSndAlias(const SndAlias& alias) const
    {
        const auto soundFile = FindSoundDataInSoundBanks(alias.assetId);
        if (soundFile.IsOpen())
        {
            const auto format = static_cast<snd_asset_format>(soundFile.m_entry.format);
            switch (format)
            {
            case SND_ASSET_FORMAT_PCMS16:
                DumpSoundFilePcm(alias.assetFileName, soundFile, 16u);
                break;

            case SND_ASSET_FORMAT_FLAC:
                DumpSoundFilePassthrough(alias.assetFileName, soundFile, ".flac");
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

    void DumpSoundData(const SndBank* sndBank) const
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
                    DumpSndAlias(alias);
                    dumpedAssets.emplace(alias.assetId);
                }
            }
        }
    }

    void DumpSndBank(const XAssetInfo<SndBank>* sndBankInfo) const
    {
        const auto* sndBank = sndBankInfo->Asset();

        DumpSndBankAliases(sndBank);
        DumpSoundData(sndBank);
    }

public:
    explicit Internal(AssetDumpingContext& context)
        : m_context(context)
    {
    }

    void DumpPool(AssetPool<SndBank>* pool) const
    {
        for (const auto* assetInfo : *pool)
        {
            if (!assetInfo->m_name.empty() && assetInfo->m_name[0] == ',')
                continue;

            DumpSndBank(assetInfo);
        }
    }
};

void AssetDumperSndBank::DumpPool(AssetDumpingContext& context, AssetPool<SndBank>* pool)
{
    const Internal internal(context);
    internal.DumpPool(pool);
}
