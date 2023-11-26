#include "AssetDumperSndBank.h"

#include "Csv/CsvStream.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "Sound/WavWriter.h"
#include "Utils/ClassUtils.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unordered_set>

using namespace T6;
namespace fs = std::filesystem;

namespace
{
    const std::string ALIAS_HEADERS[]{
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

    constexpr const char* EXTENSION_BY_FORMAT[SND_ASSET_FORMAT_COUNT]{
        ".wav",  // SND_ASSET_FORMAT_PCMS16
        ".wav",  // SND_ASSET_FORMAT_PCMS24
        ".wav",  // SND_ASSET_FORMAT_PCMS32
        ".wav",  // SND_ASSET_FORMAT_IEEE
        ".xma",  // SND_ASSET_FORMAT_XMA4
        ".mp3",  // SND_ASSET_FORMAT_MP3
        ".wav",  // SND_ASSET_FORMAT_MSADPCM
        ".wma",  // SND_ASSET_FORMAT_WMA
        ".flac", // SND_ASSET_FORMAT_FLAC
        ".wav",  // SND_ASSET_FORMAT_WIIUADPCM
        ".mpc",  // SND_ASSET_FORMAT_MPC
    };

} // namespace

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

    std::unique_ptr<std::ostream> OpenAliasOutputFile(const SndBank* sndBank) const
    {
        std::ostringstream ss;

        const char* name;
        if (sndBank->streamAssetBank.zone)
            name = sndBank->streamAssetBank.zone;
        else if (sndBank->loadAssetBank.zone)
            name = sndBank->loadAssetBank.zone;
        else if (sndBank->loadedAssets.zone)
            name = sndBank->loadedAssets.zone;
        else
            name = sndBank->name;

        ss << "soundaliases/" << name << "_aliases.csv";

        return m_context.OpenAssetFile(ss.str());
    }

    static void WriteAliasFileHeader(CsvOutputStream& stream)
    {
        for (const auto& headerField : ALIAS_HEADERS)
        {
            stream.WriteColumn(headerField);
        }

        stream.NextRow();
    }

    void WriteAliasToFile(CsvOutputStream& stream, const SndAlias* alias) const
    {
        SoundAssetBankEntry entry;
        std::string extension;
        if (FindSoundBankEntry(alias->assetId, entry))
        {
            assert(entry.format < SND_ASSET_FORMAT_COUNT);
            if (entry.format < SND_ASSET_FORMAT_COUNT)
                extension = EXTENSION_BY_FORMAT[entry.format];
        }

        // name
        WriteColumnNullSafe(stream, alias->name);

        // file
        if (alias->assetFileName)
            stream.WriteColumn(GetAssetFilename(alias->assetFileName, extension));
        else
            stream.WriteColumn("");

        // template
        stream.WriteColumn("");

        // loadspec
        stream.WriteColumn("");

        // secondary
        WriteColumnNullSafe(stream, alias->secondaryname);

        // group
        stream.WriteColumn("");

        // vol_min
        WriteColumnFloat16(stream, alias->volMin);

        // vol_max
        WriteColumnFloat16(stream, alias->volMax);

        // team_vol_mod
        stream.WriteColumn("");

        // dist_min
        WriteColumnFloat16(stream, alias->distMin);

        // dist_max
        WriteColumnFloat16(stream, alias->distMax);

        // dist_reverb_max
        WriteColumnFloat16(stream, alias->distReverbMax);

        // volume_falloff_curve
        stream.WriteColumn("");

        // reverb_falloff_curve
        stream.WriteColumn("");

        // volume_min_falloff_curve
        stream.WriteColumn("");

        // reverb_min_falloff_curve
        stream.WriteColumn("");

        // limit_count
        WriteColumnUnsignedNumeric(stream, alias->limitCount);

        // limit_type
        stream.WriteColumn("");

        // entity_limit_count
        WriteColumnUnsignedNumeric(stream, alias->entityLimitCount);

        // entity_limit_type
        stream.WriteColumn("");

        // pitch_min
        WriteColumnFloat16(stream, alias->pitchMin);

        // pitch_max
        WriteColumnFloat16(stream, alias->pitchMax);

        // team_pitch_mod
        stream.WriteColumn("");

        // min_priority
        WriteColumnUnsignedNumeric(stream, alias->minPriority);

        // max_priority
        WriteColumnUnsignedNumeric(stream, alias->maxPriority);

        // min_priority_threshold
        WriteColumnUnsignedNumeric(stream, alias->minPriorityThreshold);

        // max_priority_threshold
        WriteColumnUnsignedNumeric(stream, alias->maxPriorityThreshold);

        // spatialized
        stream.WriteColumn("");

        // type
        stream.WriteColumn("");

        // loop
        stream.WriteColumn("");

        // randomize_type
        stream.WriteColumn("");

        // probability
        WriteColumnUnsignedNumeric(stream, alias->probability);

        // start_delay
        WriteColumnUnsignedNumeric(stream, alias->startDelay);

        // reverb_send
        WriteColumnUnsignedNumeric(stream, alias->reverbSend);

        // duck
        WriteColumnUnsignedNumeric(stream, alias->duck);

        // pan
        WriteColumnUnsignedNumeric(stream, alias->pan);

        // center_send
        WriteColumnUnsignedNumeric(stream, alias->centerSend);

        // envelop_min
        WriteColumnUnsignedNumeric(stream, alias->envelopMin);

        // envelop_max
        WriteColumnUnsignedNumeric(stream, alias->envelopMax);

        // envelop_percentage
        WriteColumnUnsignedNumeric(stream, alias->envelopPercentage);

        // occlusion_level
        WriteColumnFloat8(stream, alias->occlusionLevel);

        // occlusion_wet_dry
        stream.WriteColumn("");

        // is_big
        stream.WriteColumn("");

        // distance_lpf
        stream.WriteColumn("");

        // move_type
        stream.WriteColumn("");

        // move_time
        stream.WriteColumn("");

        // real_delay
        stream.WriteColumn("");

        // subtitle
        WriteColumnNullSafe(stream, alias->subtitle);

        // mature
        stream.WriteColumn("");

        // doppler
        stream.WriteColumn("");

        // futz
        WriteColumnUnsignedNumeric(stream, alias->futzPatch);

        // context_type
        stream.WriteColumn("");

        // context_value
        stream.WriteColumn("");

        // compression
        stream.WriteColumn("");

        // timescale
        stream.WriteColumn("");

        // music
        stream.WriteColumn("");

        // fade_in
        WriteColumnSignedNumeric(stream, alias->fadeIn);

        // fade_out
        WriteColumnSignedNumeric(stream, alias->fadeOut);

        // pc_format
        stream.WriteColumn("");

        // pause
        stream.WriteColumn("");

        // stop_on_death
        stream.WriteColumn("");

        // bus
        stream.WriteColumn("");

        // snapshot
        stream.WriteColumn("");

        stream.NextRow();
    }

    static void WriteColumnNullSafe(CsvOutputStream& stream, const char* value)
    {
        if (value)
            stream.WriteColumn(value);
        else
            stream.WriteColumn("");
    }

    static void WriteColumnFloat8(CsvOutputStream& stream, const uint8_t value)
    {
        std::ostringstream ss;

        ss << std::setprecision(6) << std::fixed << (static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint8_t>::max()));

        stream.WriteColumn(ss.str());
    }

    static void WriteColumnFloat16(CsvOutputStream& stream, const uint16_t value)
    {
        std::ostringstream ss;

        ss << std::setprecision(6) << std::fixed << (static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint16_t>::max()));

        stream.WriteColumn(ss.str());
    }

    static void WriteColumnSignedNumeric(CsvOutputStream& stream, const int value)
    {
        stream.WriteColumn(std::to_string(value));
    }

    static void WriteColumnUnsignedNumeric(CsvOutputStream& stream, const unsigned int value)
    {
        stream.WriteColumn(std::to_string(value));
    }

    void DumpSndBankAliases(const SndBank* sndBank) const
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

    static bool FindSoundBankEntry(const unsigned assetId, SoundAssetBankEntry& entry)
    {
        for (const auto* soundBank : SoundBank::Repository)
        {
            if (soundBank->GetEntry(assetId, entry))
                return true;
        }

        return false;
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
        assert(soundFile.m_entry.format < SND_ASSET_FORMAT_COUNT);
        const auto outFile = OpenAssetOutputFile(assetFileName, EXTENSION_BY_FORMAT[soundFile.m_entry.format]);
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

    void DumpSoundFilePassthrough(const char* assetFileName, const SoundBankEntryInputStream& soundFile) const
    {
        assert(soundFile.m_entry.format < SND_ASSET_FORMAT_COUNT);
        const auto outFile = OpenAssetOutputFile(assetFileName, EXTENSION_BY_FORMAT[soundFile.m_entry.format]);
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
                DumpSoundFilePassthrough(alias.assetFileName, soundFile);
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
