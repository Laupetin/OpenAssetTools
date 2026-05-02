#include "SndAliasListDumperIW3.h"

#include "Csv/CsvStream.h"
#include "Game/IW3/CommonIW3.h"
#include "Game/IW3/GameIW3.h"
#include "Game/IW3/SoundConstantsIW3.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "Sound/WavWriter.h"
#include "Utils/Logging/Log.h"
#include "Zone/ZoneRegistry.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <unordered_set>

using namespace IW3;
namespace fs = std::filesystem;

namespace
{
    const std::string ALIAS_HEADERS[]{
        "name",
        "sequence",
        "file",
        "vol_min",
        "vol_max",
        "vol_mod",
        "pitch_min",
        "pitch_max",
        "dist_min",
        "dist_max",
        "channel",
        "type",
        "probability",
        "loop",
        "masterslave",
        "loadspec",
        "subtitle",
        "compression",
        "secondaryaliasname",
        "volumefalloffcurve",
        "startdelay",
        "speakermap",
        "reverb",
        "lfe percentage",
        "center percentage",
        "platform",
        "envelop_min",
        "envelop_max",
        "envelop percentage"
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

    constexpr unsigned FRAME_RATE_FOR_INDEX[]{
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

    constexpr auto FORMATTING_RETRIES = 5;

    class LoadedSoundBankHashes : public IZoneAssetDumperState
    {
    public:
        void Initialize()
        {
            for (const auto& zone : ZoneRegistry::GetRegistryForGame(GameId::T6)->Zones())
            {
                auto sndBankPool = zone->m_pools.PoolAssets<AssetSoundBank>();
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
        auto first = true;
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
        const auto dbSpl = std::clamp(Common::LinearToDbspl(linear), 0.0f, 100.0f);

        std::string dbSplFormat;
        for (auto i = 0; i < FORMATTING_RETRIES; i++)
        {
            dbSplFormat = std::format("{:.{}f}", dbSpl, i);
            const auto dbSplRound = std::stof(dbSplFormat);
            const auto dbSplRoundToValue = static_cast<uint16_t>(Common::DbsplToLinear(dbSplRound) * static_cast<float>(std::numeric_limits<uint16_t>::max()));

            if (dbSplRoundToValue == value)
                break;
        }

        stream.WriteColumn(dbSplFormat);
    }

    void WriteColumnPitchHertz(CsvOutputStream& stream, const uint16_t value)
    {
        const auto hertz = static_cast<float>(value) / static_cast<float>(std::numeric_limits<int16_t>::max());
        const auto cents = std::clamp(Common::HertzToCents(hertz), -2400.0f, 1200.0f);

        std::string centsFormat;
        for (auto i = 0; i < FORMATTING_RETRIES; i++)
        {
            centsFormat = std::format("{:.{}f}", cents, i);
            const auto centsRound = std::stof(centsFormat);
            const auto centsRoundToValue = static_cast<uint16_t>(Common::CentsToHertz(centsRound) * static_cast<float>(std::numeric_limits<int16_t>::max()));

            if (centsRoundToValue == value)
                break;
        }

        stream.WriteColumn(centsFormat);
    }

    void WriteColumnNormByte(CsvOutputStream& stream, const uint8_t value)
    {
        const auto normValue = static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint8_t>::max());

        std::string normValueFormat;
        for (auto i = 0; i < FORMATTING_RETRIES; i++)
        {
            normValueFormat = std::format("{:.{}f}", normValue, i);
            const auto normValueRound = std::stof(normValueFormat);
            const auto normValueRoundToValue = static_cast<uint8_t>(normValueRound * static_cast<float>(std::numeric_limits<uint8_t>::max()));

            if (normValueRoundToValue == value)
                break;
        }

        stream.WriteColumn(normValueFormat);
    }
    void WriteAliasToFile(CsvOutputStream& stream, const snd_alias_t& alias)
    {
        // name
        WriteColumnString(stream, alias.aliasName);

        // sequence
        WriteColumnIntegral(stream, alias.sequence);

        // file
        WriteColumnString(stream, std::format("{}/{}", alias.soundFile->u.streamSnd.dir, alias.soundFile->u.streamSnd.name));

        // vol_min
        WriteColumnVolumeLinear(stream, alias.volMin);

        // vol_max
        WriteColumnVolumeLinear(stream, alias.volMax);

        // vol_mod
        // match a string in the volumemodgroups.def file
        
        // pitch_min
        WriteColumnPitchHertz(stream, alias.pitchMin);

        // pitch_max
        WriteColumnPitchHertz(stream, alias.pitchMax);

        // dist_min
        WriteColumnIntegral(stream, alias.distMin);

        // dist_max
        WriteColumnIntegral(stream, alias.distMax);

        // channel
        // auto, menu, weapon, voice, item, body, local, music, announcer
        
        // type
        // primed, streamed, loaded
       
        // probability
        WriteColumnIntegral(stream, alias.probability);

        // loop
        // masterslave
        // loadspec
        // subtitle
        WriteColumnString(stream, alias.subtitle);

        // compression
        // secondaryaliasname
        WriteColumnString(stream, alias.secondaryAliasName);

        // volumefalloffcurve
        WriteColumnString(stream, alias.volumeFalloffCurve->filename);

        // startdelay
        WriteColumnIntegral(stream, alias.startDelay);

        // speakermap
        WriteColumnString(stream, alias.speakerMap->name);

        // reverb
        // lfe percentage
        WriteColumnIntegral(stream, alias.lfePercentage);

        // center percentage
        WriteColumnIntegral(stream, alias.centerPercentage);

        // platform
        // envelop_min
        WriteColumnIntegral(stream, alias.envelopMin);

        // envelop_max
        WriteColumnIntegral(stream, alias.envelopMax);

        // envelop percentage
        WriteColumnIntegral(stream, alias.envelopPercentage);
    }

    void DumpAliases(const AssetDumpingContext& context, const snd_alias_list_t& sndAliasList)
    {
        const auto outFile = OpenAssetOutputFile(context, std::format("soundaliases/{}", sndAliasList.aliasName), ".csv");
        if (!outFile)
        {
            con::error("Failed to open sound alias list output file: \"\"", sndAliasList.aliasName);
            return;
        }

        CsvOutputStream csvStream(*outFile);
        WriteAliasFileHeader(csvStream);

        for (auto i = 0u; i < sndAliasList.count; i++)
        {
            const auto& alias = sndAliasList.head[i];

            WriteAliasToFile(csvStream, alias);
            csvStream.NextRow();

            DumpSpeakerMap(context, alias.speakerMap);
        }
    }

    void DumpSpeakerMap(const AssetDumpingContext& context, const SpeakerMap* speakerMap)
    {
        if (!speakerMap)
            return;

        const auto outFile = OpenAssetOutputFile(context, std::format("soundaliases/{}", speakerMap->name), ".spkrmap");
        if (!outFile)
        {
            con::error("Failed to open soundspeaker map output file: \"{}\"", speakerMap->name);
            return;
        }
        std::ostream& stream(*outFile);
        stream << "SPKRMAP\r\n\r\n\r\n";

        for (int i = 0; i < std::extent_v<decltype(speakerMap->channelMaps)>; i++)
        {
            for (int j = 0; j < std::extent_v<decltype(speakerMap->channelMaps[i])>; j++)
            {
                MSSChannelMap channelMap = speakerMap->channelMaps[i][j];
                for (int k = 0; k < std::min(channelMap.speakerCount, (int)std::extent_v<decltype(channelMap.speakers)>); k++)
                {
                    MSSSpeakerLevels speakers = channelMap.speakers[k];
                    stream << speakers.numLevels << '\t' << speakers.levels[0] << '\t' << speakers.levels[1] << '\t' << speakers.speaker << '\n';
                }
                stream << '\n';
            }
        }
    }
} // namespace

namespace sound_alias_list
{
    void DumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetSound::Type>& asset)
    {
        const auto* sndAliasList = asset.Asset();

        DumpAliases(context, *sndAliasList);
    }
} // namespace sound_alias_list
