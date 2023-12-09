#include "AliasDumperCsv.h"

#include "Csv/CsvStream.h"
#include "Game/T6/AssetDumpers/AssetDumperSndBank.h"
#include "ObjContainer/SoundBank/SoundBank.h"
#include "ObjContainer/SoundBank/SoundBankTypes.h"
#include "SoundCommon.h"

#include <cassert>
#include <iomanip>
#include <memory>
#include <sstream>

using namespace T6;

namespace T6::sound
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

    AliasDumperCsv::AliasDumperCsv(const AssetDumpingContext& context)
        : AbstractAliasDumper(context)
    {
    }

    void AliasDumperCsv::WriteAliasFileHeader(CsvOutputStream& stream)
    {
        for (const auto& headerField : ALIAS_HEADERS)
        {
            stream.WriteColumn(headerField);
        }

        stream.NextRow();
    }

    void AliasDumperCsv::WriteAliasToFile(CsvOutputStream& stream, const SndAlias* alias)
    {
        SoundAssetBankEntry entry;
        std::string extension;
        if (FindSoundBankEntry(alias->assetId, entry))
        {
            assert(entry.format < SND_ASSET_FORMAT_COUNT);
            if (entry.format < SND_ASSET_FORMAT_COUNT)
                extension = sound::EXTENSION_BY_FORMAT[entry.format];
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

    void AliasDumperCsv::WriteColumnNullSafe(CsvOutputStream& stream, const char* value)
    {
        if (value)
            stream.WriteColumn(value);
        else
            stream.WriteColumn("");
    }

    void AliasDumperCsv::WriteColumnFloat8(CsvOutputStream& stream, const uint8_t value)
    {
        std::ostringstream ss;

        ss << std::setprecision(6) << std::fixed << (static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint8_t>::max()));

        stream.WriteColumn(ss.str());
    }

    void AliasDumperCsv::WriteColumnFloat16(CsvOutputStream& stream, const uint16_t value)
    {
        std::ostringstream ss;

        ss << std::setprecision(6) << std::fixed << (static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint16_t>::max()));

        stream.WriteColumn(ss.str());
    }

    void AliasDumperCsv::WriteColumnSignedNumeric(CsvOutputStream& stream, const int value)
    {
        stream.WriteColumn(std::to_string(value));
    }

    void AliasDumperCsv::WriteColumnUnsignedNumeric(CsvOutputStream& stream, const unsigned int value)
    {
        stream.WriteColumn(std::to_string(value));
    }

    void AliasDumperCsv::DumpSndBankAliases(const SndBank* sndBank) const
    {
        const auto outputFile = OpenAliasOutputFile(sndBank, ".csv");

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
} // namespace T6::sound
