#include "AssetDumperSndBank.h"

#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/SoundConstantsT6.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "Sound/WavWriter.h"
#include "nlohmann/json.hpp"

#include <cmath>
#include <filesystem>
#include <format>
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
        "duck_group",
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

    const std::string REVERB_HEADERS[]{
        "name",
        "smoothing",
        "earlyTime",
        "lateTime",
        "earlyGain",
        "lateGain",
        "returnGain",
        "earlyLpf",
        "lateLpf",
        "inputLpf",
        "dampLpf",
        "wallReflect",
        "dryGain",
        "earlySize",
        "lateSize",
        "diffusion",
        "returnHighpass",
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

    std::unordered_map<unsigned int, std::string> CreateCurvesMap()
    {
        std::unordered_map<unsigned int, std::string> result;
        for (auto i = 0u; i < std::extent_v<decltype(SOUND_CURVES)>; i++)
            result.emplace(T6::Common::SND_HashName(SOUND_CURVES[i]), SOUND_CURVES[i]);
        return result;
    }

    const std::unordered_map<unsigned int, std::string> CURVES_MAP = CreateCurvesMap();
} // namespace

class AssetDumperSndBank::Internal
{
    AssetDumpingContext& m_context;

    _NODISCARD std::string GetAssetFilename(std::string outputFileName, const std::string& extension) const
    {
        fs::path assetPath(m_context.m_base_path);

        std::ranges::replace(outputFileName, '\\', '/');
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

    static void WriteReverbFileHeader(CsvOutputStream& stream)
    {
        for (const auto& headerField : REVERB_HEADERS)
        {
            stream.WriteColumn(headerField);
        }

        stream.NextRow();
    }

    static const char* FindNameForDuck(const unsigned int id, const SndBank* bank)
    {
        for (auto i = 0u; i < bank->duckCount; i++)
        {
            if (id == bank->ducks[i].id)
            {
                return bank->ducks[i].name;
            }
        }

        return "";
    }

    static const char* ExtensionForSndFormat(const snd_asset_format format)
    {
        switch (format)
        {
        case SND_ASSET_FORMAT_PCMS16:
            return ".wav";

        case SND_ASSET_FORMAT_FLAC:
            return ".flac";

        default:
            assert(false);
            return "";
        }
    }

    static float LinearToDbspl(float linear)
    {
        linear = std::max(linear, 0.0000152879f);

        const auto db = 20.0f * std::log10(linear);
        if (db > -95.0f)
            return db + 100.0f;

        return 0;
    }

    static float HertzToCents(const float hertz)
    {
        return 1200.0f * std::log2(hertz);
    }

    static void WriteColumnVolumeLinear(CsvOutputStream& stream, const uint16_t value)
    {
        const auto linear = static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint16_t>::max());
        const auto dbSpl = std::clamp(LinearToDbspl(linear), 0.0f, 100.0f);
        stream.WriteColumn(std::format("{:.3g}", dbSpl));
    }

    static void WriteColumnPitchHertz(CsvOutputStream& stream, const uint16_t value)
    {
        const auto hertz = static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint16_t>::max());
        const auto cents = std::clamp(HertzToCents(hertz), -2400.0f, 1200.0f);
        stream.WriteColumn(std::format("{:.4g}", cents));
    }

    static void WriteAliasToFile(CsvOutputStream& stream, const SndAlias* alias, const std::optional<snd_asset_format> maybeFormat, const SndBank* bank)
    {
        // name
        stream.WriteColumn(alias->name);

        // file
        const auto* extension = maybeFormat ? ExtensionForSndFormat(*maybeFormat) : "";
        stream.WriteColumn(alias->assetFileName ? std::format("{}{}", alias->assetFileName, extension) : "");

        // template
        stream.WriteColumn("");

        // loadspec
        stream.WriteColumn("");

        // secondary
        stream.WriteColumn((alias->secondaryname && *alias->secondaryname) ? alias->secondaryname : "");

        // group
        stream.WriteColumn(SOUND_GROUPS[alias->flags.volumeGroup]);

        // vol_min
        WriteColumnVolumeLinear(stream, alias->volMin);

        // vol_max
        WriteColumnVolumeLinear(stream, alias->volMax);

        // team_vol_mod
        stream.WriteColumn("");

        // dist_min
        stream.WriteColumn(std::to_string(alias->distMin));

        // dist_max
        stream.WriteColumn(std::to_string(alias->distMax));

        // dist_reverb_max
        stream.WriteColumn(std::to_string(alias->distReverbMax));

        // volume_falloff_curve
        stream.WriteColumn(SOUND_CURVES[alias->flags.volumeFalloffCurve]);

        // reverb_falloff_curve
        stream.WriteColumn(SOUND_CURVES[alias->flags.reverbFalloffCurve]);

        // volume_min_falloff_curve
        stream.WriteColumn(SOUND_CURVES[alias->flags.volumeMinFalloffCurve]);

        // reverb_min_falloff_curve
        stream.WriteColumn(SOUND_CURVES[alias->flags.reverbMinFalloffCurve]);

        // limit_count
        stream.WriteColumn(std::to_string(alias->limitCount));

        // limit_type
        stream.WriteColumn(SOUND_LIMIT_TYPES[alias->flags.limitType]);

        // entity_limit_count
        stream.WriteColumn(std::to_string(alias->entityLimitCount));

        // entity_limit_type
        stream.WriteColumn(SOUND_LIMIT_TYPES[alias->flags.entityLimitType]);

        // pitch_min
        WriteColumnPitchHertz(stream, alias->pitchMin);

        // pitch_max
        WriteColumnPitchHertz(stream, alias->pitchMax);

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
        stream.WriteColumn(SOUND_LOAD_TYPES[alias->flags.loadType]);

        // loop
        stream.WriteColumn(alias->flags.looping == T6::SA_NON_LOOPING ? "nonlooping" : "looping");

        // randomize_type
        stream.WriteColumn(SOUND_RANDOMIZE_TYPES[std::min(alias->flags.randomizeType, 3u)]);

        // probability
        stream.WriteColumn(std::to_string(alias->probability));

        // start_delay
        stream.WriteColumn(std::to_string(alias->startDelay));

        // reverb_send
        WriteColumnVolumeLinear(stream, alias->reverbSend);

        // duck
        stream.WriteColumn(FindNameForDuck(alias->duck, bank));

        // duck_group
        stream.WriteColumn(SOUND_DUCK_GROUPS[alias->duckGroup]);

        // pan
        stream.WriteColumn(alias->flags.panType == SA_PAN_2D ? "2d" : "3d");

        // center_send
        WriteColumnVolumeLinear(stream, alias->centerSend);

        // envelop_min
        stream.WriteColumn(std::to_string(alias->envelopMin));

        // envelop_max
        stream.WriteColumn(std::to_string(alias->envelopMax));

        // envelop_percentage
        WriteColumnVolumeLinear(stream, alias->envelopPercentage);

        // occlusion_level
        stream.WriteColumn(std::to_string(alias->occlusionLevel));

        // occlusion_wet_dry
        stream.WriteColumn("");

        // is_big
        stream.WriteColumn(alias->flags.isBig ? "yes" : "no");

        // distance_lpf
        stream.WriteColumn(alias->flags.distanceLpf ? "yes" : "no");

        // move_type
        stream.WriteColumn(SOUND_MOVE_TYPES[alias->flags.fluxType]);

        // move_time
        stream.WriteColumn(std::to_string(alias->fluxTime));

        // real_delay
        stream.WriteColumn("");

        // subtitle
        stream.WriteColumn((alias->subtitle && *alias->subtitle) ? alias->subtitle : "");

        // mature
        stream.WriteColumn("");

        // doppler
        stream.WriteColumn(alias->flags.doppler ? "yes" : "no");

        // futz
        stream.WriteColumn(std::to_string(alias->futzPatch));

        // context_type
        stream.WriteColumn(std::to_string(alias->contextType));

        // context_value
        stream.WriteColumn(std::to_string(alias->contextValue));

        // compression
        stream.WriteColumn("");

        // timescale
        stream.WriteColumn(alias->flags.timescale ? "yes" : "no");

        // music
        stream.WriteColumn(alias->flags.isMusic ? "yes" : "no");

        // fade_in
        stream.WriteColumn(std::to_string(alias->fadeIn));

        // fade_out
        stream.WriteColumn(std::to_string(alias->fadeOut));

        // pc_format
        stream.WriteColumn("");

        // pause
        stream.WriteColumn(alias->flags.pauseable ? "yes" : "no");

        // stop_on_death
        stream.WriteColumn(alias->flags.stopOnDeath ? "yes" : "no");

        // bus
        stream.WriteColumn(SOUND_BUS_IDS[alias->flags.busType]);

        // snapshot
        stream.WriteColumn("");
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
            std::cerr << std::format("Failed to open sound output file: \"{}\"\n", assetFileName);
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
            std::cerr << std::format("Failed to open sound output file: \"{}\"\n", assetFileName);
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

    std::optional<snd_asset_format> DumpSndAlias(const SndAlias& alias) const
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
                std::cerr << std::format("Cannot dump sound (Unknown sound format {}): \"{}\"\n", static_cast<unsigned>(format), alias.assetFileName);
                break;

            default:
                assert(false);
                std::cerr << std::format("Cannot dump sound (Unknown sound format {}): \"{}\"\n", static_cast<unsigned>(format), alias.assetFileName);
                break;
            }

            return format;
        }

        std::cerr << std::format("Could not find data for sound \"{}\"\n", alias.assetFileName);
        return {};
    }

    void DumpSndBankAliases(const SndBank* sndBank) const
    {
        std::unordered_map<unsigned int, std::optional<snd_asset_format>> dumpedAssets;

        const auto outFile = OpenAssetOutputFile(std::format("soundbank/{}.aliases", sndBank->name), ".csv");
        if (!outFile)
        {
            std::cerr << std::format("Failed to open sound alias output file: \"\"\n", sndBank->name);
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
                std::optional<snd_asset_format> maybeFormat;

                if (alias.assetId && alias.assetFileName)
                {
                    const auto previouslyDeterminedFormat = dumpedAssets.find(alias.assetId);
                    if (previouslyDeterminedFormat == dumpedAssets.end())
                    {
                        maybeFormat = DumpSndAlias(alias);
                        dumpedAssets[alias.assetId] = maybeFormat;
                    }
                    else
                    {
                        maybeFormat = previouslyDeterminedFormat->second;
                    }
                }

                WriteAliasToFile(csvStream, &alias, maybeFormat, sndBank);
                csvStream.NextRow();
            }
        }
    }

    void DumpSoundRadverb(const SndBank* sndBank) const
    {
        if (sndBank->radverbCount <= 0)
            return;

        const auto outFile = OpenAssetOutputFile(std::format("soundbank/{}.reverbs", sndBank->name), ".csv");
        if (!outFile)
        {
            std::cerr << std::format("Failed to open sound reverb output file: \"{}\"\n", sndBank->name);
            return;
        }

        CsvOutputStream csvStream(*outFile);
        WriteReverbFileHeader(csvStream);

        for (auto i = 0u; i < sndBank->radverbCount; i++)
        {
            const auto& reverb = sndBank->radverbs[i];
            csvStream.WriteColumn(reverb.name);
            csvStream.WriteColumn(std::to_string(reverb.smoothing));
            csvStream.WriteColumn(std::to_string(reverb.earlyTime));
            csvStream.WriteColumn(std::to_string(reverb.lateTime));
            csvStream.WriteColumn(std::to_string(reverb.earlyGain));
            csvStream.WriteColumn(std::to_string(reverb.lateGain));
            csvStream.WriteColumn(std::to_string(reverb.returnGain));
            csvStream.WriteColumn(std::to_string(reverb.earlyLpf));
            csvStream.WriteColumn(std::to_string(reverb.lateLpf));
            csvStream.WriteColumn(std::to_string(reverb.inputLpf));
            csvStream.WriteColumn(std::to_string(reverb.dampLpf));
            csvStream.WriteColumn(std::to_string(reverb.wallReflect));
            csvStream.WriteColumn(std::to_string(reverb.dryGain));
            csvStream.WriteColumn(std::to_string(reverb.earlySize));
            csvStream.WriteColumn(std::to_string(reverb.lateSize));
            csvStream.WriteColumn(std::to_string(reverb.diffusion));
            csvStream.WriteColumn(std::to_string(reverb.returnHighpass));
            csvStream.NextRow();
        }
    }

    void DumpSoundDucks(const SndBank* sndBank) const
    {
        if (sndBank->duckCount <= 0)
            return;

        const auto outFile = OpenAssetOutputFile(std::format("soundbank/{}.ducklist", sndBank->name), ".csv");
        if (!outFile)
        {
            std::cerr << std::format("Failed to open sound reverb output file: \"{}\"\n", sndBank->name);
            return;
        }

        CsvOutputStream csvStream(*outFile);
        csvStream.WriteColumn("name");
        csvStream.NextRow();

        for (auto i = 0u; i < sndBank->duckCount; i++)
        {
            const auto& duck = sndBank->ducks[i];
            csvStream.WriteColumn(duck.name);
            csvStream.NextRow();

            const auto duckFile = OpenAssetOutputFile(std::format("soundbank/ducks/{}", duck.name), ".duk");
            if (!outFile)
            {
                std::cerr << std::format("Failed to open sound duck output file: \"{}\"\n", duck.name);
                return;
            }

            nlohmann::json duckObj{};
            duckObj["fadeIn"] = duck.fadeIn;
            duckObj["fadeOut"] = duck.fadeOut;
            duckObj["startDelay"] = duck.startDelay;
            duckObj["distance"] = duck.distance;
            duckObj["length"] = duck.length;
            duckObj["fadeInCurveId"] = duck.fadeInCurve;
            duckObj["fadeOutCurveId"] = duck.fadeOutCurve;
            duckObj["updateWhilePaused"] = duck.updateWhilePaused;

            auto fadeInItr = CURVES_MAP.find(duck.fadeInCurve);
            if (fadeInItr != CURVES_MAP.end())
            {
                duckObj["fadeInCurve"] = fadeInItr->second;
            }

            auto fadeOutItr = CURVES_MAP.find(duck.fadeOutCurve);
            if (fadeOutItr != CURVES_MAP.end())
            {
                duckObj["fadeOutCurve"] = fadeOutItr->second;
            }

            auto values = std::vector<nlohmann::json>{};
            for (auto j = 0u; j < 32u; j++)
            {
                values.push_back({
                    {"duckGroup",   SOUND_DUCK_GROUPS[j]},
                    {"attenuation", duck.attenuation[j] },
                    {"filter",      duck.filter[j]      }
                });
            }

            duckObj["values"] = values;
            *duckFile << duckObj.dump(4) << "\n";
        }
    }

    void DumpSndBank(const XAssetInfo<SndBank>* sndBankInfo) const
    {
        const auto* sndBank = sndBankInfo->Asset();

        DumpSndBankAliases(sndBank);
        DumpSoundRadverb(sndBank);
        DumpSoundDucks(sndBank);
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
