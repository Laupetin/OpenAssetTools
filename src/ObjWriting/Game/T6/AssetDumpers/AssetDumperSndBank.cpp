#include "AssetDumperSndBank.h"

#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/GameT6.h"
#include "Game/T6/SoundConstantsT6.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "Sound/WavWriter.h"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>
#include <unordered_set>

using namespace T6;
namespace fs = std::filesystem;

namespace
{
    const std::string ALIAS_HEADERS[]{
        "Name",
        "FileSource",
        "Secondary",
        "Subtitle",
        "VolMin",
        "VolMax",
        "PitchMin",
        "PitchMax",
        "DistMin",
        "DistMaxDry",
        "DistMaxWet",
        "Probability",
        "EnvelopMin",
        "EnvelopMax",
        "EnvelopPercentage",
        "CenterSend",
        "ReverbSend",
        "StartDelay",
        "PriorityThresholdMin",
        "PriorityThresholdMax",
        "OcclusionLevel",
        "FluxTime",
        "Duck",
        "PriorityMin",
        "PriorityMax",
        "LimitCount",
        "EntityLimitCount",
        "DryMinCurve",
        "DryMaxCurve",
        "WetMinCurve",
        "WetMaxCurve",
        "Pan",
        "DuckGroup",
        "ContextType",
        "ContextValue",
        "FadeIn",
        "FadeOut",
        "StopOnPlay",
        "DopplerScale",
        "FutzPatch",
        "LimitType",
        "EntityLimitType",
        "RandomizeType",
        "FluxType",
        "Storage",
        "VolumeGroup",
        "DistanceLpf",
        "Doppler",
        "IsBig",
        "Looping",
        "PanType",
        "IsMusic",
        "Timescale",
        "Pauseable",
        "StopOnEntDeath",
        "Bus",
        "VoiceLimit",
        "IgnoreMaxDist",
        "NeverPlayTwice",
        "IsCinematic",
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

    constexpr const char* KNOWN_CONTEXT_TYPES[]{
        "",
        "plr_stance",
        "grass",
        "f35",
        "ringoff_plr",
        "mature",
    };

    constexpr const char* KNOWN_CONTEXT_VALUES[]{
        "",
        "stand",
        "crouch",
        "prone",
        "no_grass",
        "in_grass",
        "interior",
        "exterior",
        "outdoor",
        "indoor",
        "safe",
        "explicit",
    };

    constexpr const char* KNOWN_FUTZ_IDS[]{
        "",
        "bfutz",
        "default",
        "defaultbkp",
        "dlc_res_1",
        "dlc_res_2",
        "dlc_res_3",
        "dlc_res_4",
        "dlc_res_5",
        "dlc_res_6",
        "dlc_res_7",
        "dlc_res_8",
        "good_1",
        "jet_wing_helmet",
        "jet_wing_helmet_flying",
        "mpl_agr_pov",
        "mpl_chopper_pov",
        "mpl_quad_pov",
        "mpl_reaper_pov",
        "no_gfutz",
        "spl_asd_pov",
        "spl_bigdog_pov",
        "spl_heli_future",
        "spl_quad_pov",
        "spl_spiderbot_pov",
        "spl_spymic",
        "spl_tow_missile",
        "spl_turret",
        "spl_war_command",
        "test_1",
        "test_2",
        "tueyeckert",
    };

    std::unordered_map<unsigned int, std::string> CreateSoundHashMap(const char* const* values, const unsigned valueCount)
    {
        std::unordered_map<unsigned int, std::string> result;
        for (auto i = 0u; i < valueCount; i++)
            result.emplace(Common::SND_HashName(values[i]), values[i]);
        return result;
    }

    const auto CURVES_MAP = CreateSoundHashMap(SOUND_CURVES, std::extent_v<decltype(SOUND_CURVES)>);
    const auto CONTEXT_TYPES_MAP = CreateSoundHashMap(KNOWN_CONTEXT_TYPES, std::extent_v<decltype(KNOWN_CONTEXT_TYPES)>);
    const auto CONTEXT_VALUES_MAP = CreateSoundHashMap(KNOWN_CONTEXT_VALUES, std::extent_v<decltype(KNOWN_CONTEXT_VALUES)>);
    const auto FUTZ_IDS_MAP = CreateSoundHashMap(KNOWN_FUTZ_IDS, std::extent_v<decltype(KNOWN_FUTZ_IDS)>);

    class LoadedSoundBankHashes
    {
    public:
        void Initialize()
        {
            for (const auto& zone : g_GameT6.GetZones())
            {
                auto& sndBankPool = *dynamic_cast<GameAssetPoolT6*>(zone->m_pools.get())->m_sound_bank;
                for (auto* entry : sndBankPool)
                {
                    const auto& sndBank = *entry->Asset();

                    m_alias_names.reserve(m_alias_names.size() + sndBank.aliasCount);
                    for (auto aliasIndex = 0u; aliasIndex < sndBank.aliasCount; aliasIndex++)
                    {
                        const auto& alias = sndBank.alias[aliasIndex];
                        m_alias_names.emplace(alias.id, alias.name);
                    }

                    m_duck_names.reserve(m_duck_names.size() + sndBank.duckCount);
                    for (auto duckIndex = 0u; duckIndex < sndBank.duckCount; duckIndex++)
                    {
                        const auto& duck = sndBank.ducks[duckIndex];
                        m_duck_names.emplace(duck.id, duck.name);
                    }
                }
            }
        }

        [[nodiscard]] std::optional<const char*> GetAliasName(const unsigned hash) const
        {
            if (hash == 0)
                return "";

            const auto knownAliasHash = m_alias_names.find(hash);
            if (knownAliasHash != m_alias_names.end())
                return knownAliasHash->second;
            return std::nullopt;
        }

        [[nodiscard]] std::optional<const char*> GetDuckName(const unsigned hash) const
        {
            if (hash == 0)
                return "";

            const auto knownDuckHash = m_duck_names.find(hash);
            if (knownDuckHash != m_duck_names.end())
                return knownDuckHash->second;
            return std::nullopt;
        }

    private:
        std::unordered_map<unsigned, const char*> m_alias_names;
        std::unordered_map<unsigned, const char*> m_duck_names;
    };

    [[nodiscard]] std::string GetAssetFilename(const AssetDumpingContext& context, std::string outputFileName, const std::string& extension)
    {
        fs::path assetPath(context.m_base_path);

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

    std::unique_ptr<std::ostream> OpenAssetOutputFile(const AssetDumpingContext& context, const std::string& outputFileName, const std::string& extension)
    {
        fs::path assetPath(GetAssetFilename(context, outputFileName, extension));

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

    void WriteAliasFileHeader(CsvOutputStream& stream)
    {
        for (const auto& headerField : ALIAS_HEADERS)
        {
            stream.WriteColumn(headerField);
        }

        stream.NextRow();
    }

    void WriteReverbFileHeader(CsvOutputStream& stream)
    {
        for (const auto& headerField : REVERB_HEADERS)
        {
            stream.WriteColumn(headerField);
        }

        stream.NextRow();
    }

    const char* ExtensionForSndFormat(const snd_asset_format format)
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

    float LinearToDbspl(float linear)
    {
        linear = std::max(linear, 0.0000152879f);

        const auto db = 20.0f * std::log10(linear);
        if (db > -95.0f)
            return db + 100.0f;

        return 0;
    }

    float HertzToCents(const float hertz)
    {
        return 1200.0f * std::log2(hertz);
    }

    void WriteColumnString(CsvOutputStream& stream, const std::string& stringValue)
    {
        stream.WriteColumn(stringValue);
    }

    void WriteColumnString(CsvOutputStream& stream, const char* stringValue)
    {
        stream.WriteColumn(stringValue ? std::string(stringValue) : std::string());
    }

    template<typename T> void WriteColumnIntegral(CsvOutputStream& stream, const T value)
    {
        stream.WriteColumn(std::format("{}", value));
    }

    void WriteColumnEnumWithSize(CsvOutputStream& stream, const unsigned value, const char* const* enumValues, const size_t enumValueCount)
    {
        assert(value < enumValueCount);
        stream.WriteColumn(value < enumValueCount ? enumValues[value] : "");
    }

    template<size_t Size> void WriteColumnEnum(CsvOutputStream& stream, const unsigned value, const char* const (&enumValues)[Size])
    {
        WriteColumnEnumWithSize(stream, value, enumValues, Size);
    }

    void WriteColumnEnumFlagsWithSize(CsvOutputStream& stream, const unsigned value, const char* const* enumValues, const size_t enumValueCount)
    {
        assert(enumValueCount <= 32u);
        std::ostringstream ss;
        auto first = false;
        for (auto i = 0u; i < enumValueCount; i++)
        {
            const auto flagValue = 1u << i;
            if (value & flagValue)
            {
                if (first)
                    first = false;
                else
                    ss << ' ';

                ss << enumValues[i];
            }
        }

        stream.WriteColumn(ss.str());
    }

    template<size_t Size> void WriteColumnEnumFlags(CsvOutputStream& stream, const unsigned value, const char* const (&enumValues)[Size])
    {
        WriteColumnEnumFlagsWithSize(stream, value, enumValues, Size);
    }

    void WriteColumnVolumeLinear(CsvOutputStream& stream, const uint16_t value)
    {
        const auto linear = static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint16_t>::max());
        const auto dbSpl = std::clamp(LinearToDbspl(linear), 0.0f, 100.0f);
        stream.WriteColumn(std::format("{:.3g}", dbSpl));
    }

    void WriteColumnPitchHertz(CsvOutputStream& stream, const uint16_t value)
    {
        const auto hertz = static_cast<float>(value) / static_cast<float>(std::numeric_limits<int16_t>::max());
        const auto cents = std::clamp(HertzToCents(hertz), -2400.0f, 1200.0f);
        stream.WriteColumn(std::format("{:.4g}", cents));
    }

    void WriteColumnNormByte(CsvOutputStream& stream, const uint8_t value)
    {
        const auto normValue = static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint8_t>::max());
        stream.WriteColumn(std::format("{:.3g}", normValue));
    }

    void WriteColumnWithKnownHashes(CsvOutputStream& stream, const std::unordered_map<unsigned, std::string>& knownValues, const unsigned value)
    {
        const auto knownValue = knownValues.find(value);
        if (knownValue != knownValues.end())
            stream.WriteColumn(knownValue->second);
        else
            stream.WriteColumn(std::format("@{:x}", value));
    }

    void WriteColumnWithAliasHash(CsvOutputStream& stream, const LoadedSoundBankHashes& hashes, const unsigned value)
    {
        const auto name = hashes.GetAliasName(value);
        if (name)
            stream.WriteColumn(*name);
        else
            stream.WriteColumn(std::format("@{:x}", value));
    }

    void WriteColumnWithDuckHash(CsvOutputStream& stream, const LoadedSoundBankHashes& hashes, const unsigned value)
    {
        const auto name = hashes.GetDuckName(value);
        if (name)
            stream.WriteColumn(*name);
        else
            stream.WriteColumn(std::format("@{:x}", value));
    }

    void
        WriteAliasToFile(CsvOutputStream& stream, const SndAlias& alias, const std::optional<snd_asset_format> maybeFormat, const LoadedSoundBankHashes& hashes)
    {
        // Name
        WriteColumnString(stream, alias.name);

        // FileSource
        const auto* extension = maybeFormat ? ExtensionForSndFormat(*maybeFormat) : "";
        WriteColumnString(stream, alias.assetFileName ? std::format("{}{}", alias.assetFileName, extension) : "");

        // Secondary
        WriteColumnString(stream, alias.secondaryName);

        // Subtitle
        WriteColumnString(stream, alias.subtitle);

        // VolMin
        WriteColumnVolumeLinear(stream, alias.volMin);

        // VolMax
        WriteColumnVolumeLinear(stream, alias.volMax);

        // PitchMin
        WriteColumnPitchHertz(stream, alias.pitchMin);

        // PitchMax
        WriteColumnPitchHertz(stream, alias.pitchMax);

        // DistMin
        WriteColumnIntegral(stream, alias.distMin);

        // DistMaxDry
        WriteColumnIntegral(stream, alias.distMax);

        // DistMaxWet
        WriteColumnIntegral(stream, alias.distReverbMax);

        // Probability
        WriteColumnNormByte(stream, alias.probability);

        // EnvelopMin
        WriteColumnIntegral(stream, alias.envelopMin);

        // EnvelopMax
        WriteColumnIntegral(stream, alias.envelopMax);

        // EnvelopPercentage
        WriteColumnVolumeLinear(stream, alias.envelopPercentage);

        // CenterSend
        WriteColumnVolumeLinear(stream, alias.centerSend);

        // ReverbSend
        WriteColumnVolumeLinear(stream, alias.reverbSend);

        // StartDelay
        WriteColumnIntegral(stream, alias.startDelay);

        // PriorityThresholdMin
        WriteColumnNormByte(stream, alias.minPriorityThreshold);

        // PriorityThresholdMax
        WriteColumnNormByte(stream, alias.maxPriorityThreshold);

        // OcclusionLevel
        WriteColumnNormByte(stream, alias.occlusionLevel);

        // FluxTime
        WriteColumnIntegral(stream, alias.fluxTime);

        // Duck
        WriteColumnWithDuckHash(stream, hashes, alias.duck);

        // PriorityMin
        WriteColumnIntegral(stream, alias.minPriority);

        // PriorityMax
        WriteColumnIntegral(stream, alias.maxPriority);

        // LimitCount
        WriteColumnIntegral(stream, alias.limitCount);

        // EntityLimitCount
        WriteColumnIntegral(stream, alias.entityLimitCount);

        // DryMinCurve
        WriteColumnEnum(stream, alias.flags.volumeMinFalloffCurve, SOUND_CURVES);

        // DryMaxCurve
        WriteColumnEnum(stream, alias.flags.volumeFalloffCurve, SOUND_CURVES);

        // WetMinCurve
        WriteColumnEnum(stream, alias.flags.reverbMinFalloffCurve, SOUND_CURVES);

        // WetMaxCurve
        WriteColumnEnum(stream, alias.flags.reverbFalloffCurve, SOUND_CURVES);

        // Pan
        WriteColumnEnum(stream, alias.pan, SOUND_PANS);

        // DuckGroup
        WriteColumnEnum(stream, alias.duckGroup, SOUND_DUCK_GROUPS);

        // ContextType
        WriteColumnWithKnownHashes(stream, CONTEXT_TYPES_MAP, alias.contextType);

        // ContextValue
        WriteColumnWithKnownHashes(stream, CONTEXT_VALUES_MAP, alias.contextValue);

        // FadeIn
        WriteColumnIntegral(stream, alias.fadeIn);

        // FadeOut
        WriteColumnIntegral(stream, alias.fadeOut);

        // StopOnPlay
        WriteColumnWithAliasHash(stream, hashes, alias.stopOnPlay);

        // DopplerScale
        WriteColumnIntegral(stream, alias.dopplerScale);

        // FutzPatch
        WriteColumnWithKnownHashes(stream, FUTZ_IDS_MAP, alias.futzPatch);

        // LimitType
        WriteColumnEnum(stream, alias.flags.limitType, SOUND_LIMIT_TYPES);

        // EntityLimitType
        WriteColumnEnum(stream, alias.flags.entityLimitType, SOUND_LIMIT_TYPES);

        // RandomizeType
        WriteColumnEnumFlags(stream, alias.flags.randomizeType, SOUND_RANDOMIZE_TYPES);

        // FluxType
        WriteColumnEnum(stream, alias.flags.fluxType, SOUND_MOVE_TYPES);

        // Storage
        WriteColumnEnum(stream, alias.flags.loadType, SOUND_LOAD_TYPES);

        // VolumeGroup
        WriteColumnEnum(stream, alias.flags.volumeGroup, SOUND_GROUPS);

        // DistanceLpf
        WriteColumnEnum(stream, alias.flags.distanceLpf, SOUND_NO_YES);

        // Doppler
        WriteColumnEnum(stream, alias.flags.doppler, SOUND_NO_YES);

        // IsBig
        WriteColumnEnum(stream, alias.flags.isBig, SOUND_NO_YES);

        // Looping
        WriteColumnEnum(stream, alias.flags.looping, SOUND_LOOP_TYPES);

        // PanType
        WriteColumnEnum(stream, alias.flags.panType, SOUND_PAN_TYPES);

        // IsMusic
        WriteColumnEnum(stream, alias.flags.isMusic, SOUND_NO_YES);

        // Timescale
        WriteColumnEnum(stream, alias.flags.timescale, SOUND_NO_YES);

        // Pausable
        WriteColumnEnum(stream, alias.flags.pausable, SOUND_NO_YES);

        // StopOnEntDeath
        WriteColumnEnum(stream, alias.flags.stopOnEntDeath, SOUND_NO_YES);

        // Bus
        WriteColumnEnum(stream, alias.flags.busType, SOUND_BUS_IDS);

        // VoiceLimit
        WriteColumnEnum(stream, alias.flags.voiceLimit, SOUND_NO_YES);

        // IgnoreMaxDist
        WriteColumnEnum(stream, alias.flags.ignoreMaxDist, SOUND_NO_YES);

        // NeverPlayTwice
        WriteColumnEnum(stream, alias.flags.neverPlayTwice, SOUND_NO_YES);

        // IsCinematic
        WriteColumnEnum(stream, alias.flags.isCinematic, SOUND_NO_YES);
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
        const auto outFile = OpenAssetOutputFile(context, assetFileName, ".wav");
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

    void DumpSoundFilePassthrough(const AssetDumpingContext& context,
                                  const char* assetFileName,
                                  const SoundBankEntryInputStream& soundFile,
                                  const std::string& extension)
    {
        const auto outFile = OpenAssetOutputFile(context, assetFileName, extension);
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

    [[nodiscard]] std::optional<snd_asset_format> DumpSndAlias(const AssetDumpingContext& context, const SndAlias& alias)
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
                DumpSoundFilePassthrough(context, alias.assetFileName, soundFile, ".flac");
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

    void DumpSndBankAliases(const AssetDumpingContext& context, const LoadedSoundBankHashes& hashes, const SndBank& sndBank)
    {
        std::unordered_map<unsigned int, std::optional<snd_asset_format>> dumpedAssets;

        const auto outFile = OpenAssetOutputFile(context, std::format("soundbank/{}.aliases", sndBank.name), ".csv");
        if (!outFile)
        {
            std::cerr << std::format("Failed to open sound alias output file: \"\"\n", sndBank.name);
            return;
        }

        CsvOutputStream csvStream(*outFile);
        WriteAliasFileHeader(csvStream);

        for (auto i = 0u; i < sndBank.aliasCount; i++)
        {
            const auto& aliasList = sndBank.alias[i];

            for (auto j = 0; j < aliasList.count; j++)
            {
                const auto& alias = aliasList.head[j];
                std::optional<snd_asset_format> maybeFormat;

                if (alias.assetId && alias.assetFileName)
                {
                    const auto previouslyDeterminedFormat = dumpedAssets.find(alias.assetId);
                    if (previouslyDeterminedFormat == dumpedAssets.end())
                    {
                        maybeFormat = DumpSndAlias(context, alias);
                        dumpedAssets[alias.assetId] = maybeFormat;
                    }
                    else
                    {
                        maybeFormat = previouslyDeterminedFormat->second;
                    }
                }

                WriteAliasToFile(csvStream, alias, maybeFormat, hashes);
                csvStream.NextRow();
            }
        }
    }

    void DumpSoundRadverb(const AssetDumpingContext& context, const SndBank& sndBank)
    {
        if (sndBank.radverbCount <= 0)
            return;

        const auto outFile = OpenAssetOutputFile(context, std::format("soundbank/{}.reverbs", sndBank.name), ".csv");
        if (!outFile)
        {
            std::cerr << std::format("Failed to open sound reverb output file: \"{}\"\n", sndBank.name);
            return;
        }

        CsvOutputStream csvStream(*outFile);
        WriteReverbFileHeader(csvStream);

        for (auto i = 0u; i < sndBank.radverbCount; i++)
        {
            const auto& reverb = sndBank.radverbs[i];
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

    void DumpSoundDucks(const AssetDumpingContext& context, const SndBank& sndBank)
    {
        if (sndBank.duckCount <= 0)
            return;

        const auto outFile = OpenAssetOutputFile(context, std::format("soundbank/{}.ducklist", sndBank.name), ".csv");
        if (!outFile)
        {
            std::cerr << std::format("Failed to open sound reverb output file: \"{}\"\n", sndBank.name);
            return;
        }

        CsvOutputStream csvStream(*outFile);
        csvStream.WriteColumn("name");
        csvStream.NextRow();

        for (auto i = 0u; i < sndBank.duckCount; i++)
        {
            const auto& duck = sndBank.ducks[i];
            csvStream.WriteColumn(duck.name);
            csvStream.NextRow();

            const auto duckFile = OpenAssetOutputFile(context, std::format("soundbank/ducks/{}", duck.name), ".duk");
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

    void DumpSndBank(const AssetDumpingContext& context, const LoadedSoundBankHashes& hashes, const XAssetInfo<SndBank>& sndBankInfo)
    {
        const auto* sndBank = sndBankInfo.Asset();

        DumpSndBankAliases(context, hashes, *sndBank);
        DumpSoundRadverb(context, *sndBank);
        DumpSoundDucks(context, *sndBank);
    }
} // namespace

void AssetDumperSndBank::DumpPool(AssetDumpingContext& context, AssetPool<SndBank>* pool)
{
    LoadedSoundBankHashes soundBankHashes;
    soundBankHashes.Initialize();
    for (const auto* assetInfo : *pool)
    {
        if (!assetInfo->m_name.empty() && assetInfo->m_name[0] == ',')
            continue;

        DumpSndBank(context, soundBankHashes, *assetInfo);
    }
}
