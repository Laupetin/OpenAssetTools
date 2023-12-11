#include "AssetDumperSndBank.h"

#include "Csv/CsvStream.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "Sound/WavWriter.h"
#include "Utils/ClassUtils.h"

#include <filesystem>
#include <fstream>
#include <unordered_set>

using namespace T6;
namespace fs = std::filesystem;

namespace
{
    const std::string ALIAS_HEADERS[]{
        "name",
        "file",
        "template",
        "loadspec",
        "secondary",
        "group",
        "vol_min",
        "vol_max",
        "team_vol_mod",
        "dist_min",
        "dist_max",
        "dist_reverb_max",
        "volume_falloff_curve",
        "reverb_falloff_curve",
        "volume_min_falloff_curve",
        "reverb_min_falloff_curve",
        "limit_count",
        "limit_type",
        "entity_limit_count",
        "entity_limit_type",
        "pitch_min",
        "pitch_max",
        "team_pitch_mod",
        "min_priority",
        "max_priority",
        "min_priority_threshold",
        "max_priority_threshold",
        "spatialized",
        "type",
        "loop",
        "randomize_type",
        "probability",
        "start_delay",
        "reverb_send",
        "duck",
        "pan",
        "center_send",
        "envelop_min",
        "envelop_max",
        "envelop_percentage",
        "occlusion_level",
        "occlusion_wet_dry",
        "is_big",
        "distance_lpf",
        "move_type",
        "move_time",
        "real_delay",
        "subtitle",
        "mature",
        "doppler",
        "futz",
        "context_type",
        "context_value",
        "compression",
        "timescale",
        "music",
        "fade_in",
        "fade_out",
        "pc_format",
        "pause",
        "stop_on_death",
        "bus",
        "snapshot",
    };

    const std::string PREFIXES_TO_DROP[]{
        "raw/",
        "devraw/",
    };

    constexpr size_t FRAME_RATE_FOR_INDEX[]{
        8000,
        12000,
        16000,
        24000,
        32000,
        44100,
        48000,
        96000,
        192000,
    };

    const std::string GROUPS_ENUM[]
    {
        "grp_reference", "grp_master", "grp_wpn_lfe", "grp_lfe", "grp_hdrfx", "grp_music", "grp_voice", "grp_set_piece", "grp_igc", 
        "grp_mp_game", "grp_explosion", "grp_player_impacts", "grp_scripted_moment", "grp_menu", "grp_whizby", "grp_weapon", "grp_vehicle", 
        "grp_impacts", "grp_foley", "grp_destructible", "grp_physics", "grp_ambience", "grp_alerts", "grp_air", "grp_bink", "grp_announcer",
        ""
    };

    const std::string CURVES_ENUM[]
    {
        "default", "defaultmin", "allon", "alloff", "rcurve0", "rcurve1", "rcurve2", "rcurve3", "rcurve4", "rcurve5", "steep", 
        "sindelay", "cosdelay", "sin", "cos", "rev60", "rev65",
        ""
    };

    const std::string LOOP_TYPES_ENUM[]
    {
        "nonlooping", "looping"
    };

    const std::string LIMIT_TYPES_ENUM[]
    {
        "none", "oldest", "reject", "priority"
    };

    const std::string MOVE_TYPES_ENUM[]
    {
        "none", "left_player", "center_player", "right_player", "random", "left_shot", "center_shot", "right_shot"
    };

    const std::string LOAD_TYPES_ENUM[]
    {
        "unknown", "loaded", "streamed", "primed"
    };

    const std::string BUS_IDS_ENUM[]
    {
        "bus_reverb", "bus_fx", "bus_voice", "bus_pfutz", "bus_hdrfx", "bus_ui", "bus_reference", "bus_music", "bus_movie", "bus_reference", ""
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

    static void WriteAliasFileHeader(CsvOutputStream& stream)
    {
        for (const auto& headerField : ALIAS_HEADERS)
        {
            stream.WriteColumn(headerField);
        }

        stream.NextRow();
    }

    static const char* FindNameForDuck(unsigned int id, const SndDuck* ducks, unsigned int duckCount)
    {
        for (auto i = 0u; i < duckCount; i++)
        {
            if (id == ducks[i].id)
            {
                return ducks[i].name;
            }
        }

        return "";
    }

    static void WriteAliasToFile(CsvOutputStream& stream, const SndAlias* alias, const SndBank* bank)
    {
        // name
        stream.WriteColumn(alias->name);

        // file
        stream.WriteColumn(alias->assetFileName);

        // template
        stream.WriteColumn("");

        // loadspec
        stream.WriteColumn("");

        // secondary
        //stream.WriteColumn(alias->secondaryname);

        // group
        stream.WriteColumn(GROUPS_ENUM[std::min((alias->flags0 >> 17) & 0x1F, 26u)]);

        // vol_min
        stream.WriteColumn(std::to_string(alias->volMin));

        // vol_max
        stream.WriteColumn(std::to_string(alias->volMax));

        // team_vol_mod
        stream.WriteColumn("");

        // dist_min
        stream.WriteColumn(std::to_string(alias->distMin));

        // dist_max
        stream.WriteColumn(std::to_string(alias->distMax));

        // dist_reverb_max
        stream.WriteColumn(std::to_string(alias->distReverbMax));

        // volume_falloff_curve
        stream.WriteColumn(CURVES_ENUM[std::min((alias->flags1 >> 14) & 0x1F, 17u)]);

        // reverb_falloff_curve
        stream.WriteColumn(CURVES_ENUM[std::min((alias->flags1 >> 20) & 0x1F, 17u)]);

        // volume_min_falloff_curve
        stream.WriteColumn(CURVES_ENUM[std::min((alias->flags1 >> 2) & 0x1F, 17u)]);

        // reverb_min_falloff_curve"
        stream.WriteColumn(CURVES_ENUM[std::min((alias->flags1 >> 8) & 0x1F, 17u)]);

        // limit_count
        stream.WriteColumn(std::to_string(alias->limitCount));

        // limit_type
        stream.WriteColumn(LIMIT_TYPES_ENUM[(alias->flags0 >> 25) & 0x3]);

        // entity_limit_count
        stream.WriteColumn(std::to_string(alias->entityLimitCount));

        // entity_limit_type
        stream.WriteColumn(LIMIT_TYPES_ENUM[(alias->flags0 >> 27) & 0x3]);

        // pitch_min
        stream.WriteColumn(std::to_string(alias->pitchMin));
        
        // pitch_max
        stream.WriteColumn(std::to_string(alias->pitchMax));

        // team_pitch_mod
        stream.WriteColumn("");

        // min_priority
        stream.WriteColumn(std::to_string(alias->minPriority));

        // max_priority
        stream.WriteColumn(std::to_string(alias->maxPriority));

        // min_priority_threshold
        stream.WriteColumn(std::to_string(alias->minPriorityThreshold));

        // max_priority_threshold
        stream.WriteColumn(std::to_string(alias->maxPriorityThreshold));

        // spatialized
        stream.WriteColumn("");

        // type
        stream.WriteColumn(std::to_string(alias->contextType));

        // loop
        stream.WriteColumn((alias->flags0 & 0x1) == 0 ? "nonlooping" : "looping");

        // randomize_type
        stream.WriteColumn(LIMIT_TYPES_ENUM[(alias->flags0 >> 15) & 0x3]);

        // probability",
        stream.WriteColumn(std::to_string(alias->probability));

        // start_delay",
        stream.WriteColumn(std::to_string(alias->startDelay));

        // reverb_send",
        stream.WriteColumn(std::to_string(alias->reverbSend));
        
        // duck",
        stream.WriteColumn(""); // FindNameForDuck(alias->duck, bank->ducks, bank->duckCount));

        // pan",
        stream.WriteColumn(((alias->flags0 >> 6) & 0x1) == 0 ? "2d" : "3d");

        // center_send",
        stream.WriteColumn(std::to_string(alias->centerSend));

        // envelop_min",
        stream.WriteColumn(std::to_string(alias->envelopMin));

        // envelop_max",
        stream.WriteColumn(std::to_string(alias->envelopMax));

        // envelop_percentage",
        stream.WriteColumn(std::to_string(alias->envelopPercentage));

        // occlusion_level",
        stream.WriteColumn(std::to_string(alias->occlusionLevel));

        // occlusion_wet_dry",
        stream.WriteColumn("");

        // is_big",
        stream.WriteColumn(((alias->flags0 >> 4) & 0x1) == 0 ? "no" : "yes");

        // distance_lpf"
        stream.WriteColumn(((alias->flags0 >> 2) & 0x1) == 0 ? "no" : "yes");

        // move_type",
        stream.WriteColumn(MOVE_TYPES_ENUM[std::min((alias->flags0 >> 22) & 0x7, 7u)]);

        // move_time",
        stream.WriteColumn(std::to_string(alias->fluxTime));

        // real_delay",
        stream.WriteColumn("");

        // subtitle",
        //stream.WriteColumn(alias->subtitle);

        // mature",
        stream.WriteColumn("");

        // doppler",
        stream.WriteColumn(((alias->flags0 >> 1) & 0x1) == 0 ? "no" : "yes");

        // futz",
        stream.WriteColumn("");

        // context_type",
        stream.WriteColumn("hash_" + std::to_string(alias->contextType));

        // context_value",
        stream.WriteColumn("hash_" + std::to_string(alias->contextValue));

        // compression",
        stream.WriteColumn("");

        // timescale",
        stream.WriteColumn(((alias->flags0 >> 8) & 0x1) == 0 ? "no" : "yes");

        // music",
        stream.WriteColumn(((alias->flags0 >> 10) & 0x1) == 0 ? "no" : "yes");

        // fade_in",
        stream.WriteColumn(std::to_string(alias->fadeIn));

        // fade_out",
        stream.WriteColumn(std::to_string(alias->fadeOut));

        // pc_format",
        stream.WriteColumn("");

        // pause",
        stream.WriteColumn(((alias->flags0 >> 5) & 0x1) == 0 ? "no" : "yes");

        // stop_on_death",
        stream.WriteColumn(((alias->flags0 >> 7) & 0x1) == 0 ? "no" : "yes");

        // bus",
        stream.WriteColumn(BUS_IDS_ENUM[std::min((alias->flags0 >> 13) & 0xF, 10u)]);

        // snapshot",
        stream.WriteColumn("");
    }

    void DumpSndBankAliases(const SndBank* sndBank) const
    {
        const auto outFile = OpenAssetOutputFile("soundaliases\\" + std::string(sndBank->name), ".csv");
        if (!outFile)
        {
            std::cerr << "Failed to open sound output file: \"" << sndBank->name << "\"\n";
            return;
        }

        CsvOutputStream csvStream(*outFile);
        WriteAliasFileHeader(csvStream);

        for (auto i = 0u; i < sndBank->aliasCount; i++)
        {
            const auto& aliasList = sndBank->alias[i];

            for (auto j = 0; j < aliasList.count; j++)
            {
                const auto& alias = aliasList.head[j];
                if (alias.assetId && alias.assetFileName)
                {
                    WriteAliasToFile(csvStream, &alias, sndBank);
                    csvStream.NextRow();
                }
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
