#include "LoaderSoundBankT6.h"

#include "Csv/CsvHeaderRow.h"
#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/SoundConstantsT6.h"
#include "Game/T6/T6.h"
#include "ObjContainer/SoundBank/SoundBankWriter.h"
#include "Pool/GlobalAssetPool.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <cmath>
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace T6;
namespace fs = std::filesystem;

namespace
{
    const std::string PREFIXES_TO_DROP[]{
        "raw/",
        "devraw/",
    };

    [[nodiscard]] std::string GetSoundFilePath(const SndAlias* sndAlias)
    {
        std::string soundFilePath(sndAlias->assetFileName);

        std::ranges::replace(soundFilePath, '\\', '/');
        for (const auto& droppedPrefix : PREFIXES_TO_DROP)
        {
            if (soundFilePath.rfind(droppedPrefix, 0) != std::string::npos)
            {
                soundFilePath.erase(0, droppedPrefix.size());
                break;
            }
        }

        return soundFilePath;
    }

    [[nodiscard]] std::unique_ptr<std::ofstream> OpenSoundBankOutputFile(const std::string& bankName)
    {
        fs::path assetPath = SoundBankWriter::OutputPath / bankName;

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

    size_t GetValueIndex(const std::string& value, const char* const* lookupTable, const size_t len)
    {
        if (value.empty())
            return 0;

        for (auto i = 0u; i < len; i++)
        {
            if (lookupTable[i] == value)
                return i;
        }

        return 0;
    }

    bool ReadColumnString(const std::vector<CsvCell>& row, const unsigned columnIndex, const char*& value, MemoryManager& memory)
    {
        const auto& cell = row[columnIndex];
        if (!cell.m_value.empty())
            value = memory.Dup(cell.m_value.c_str());
        else
            value = nullptr;

        return true;
    }

    bool ReadColumnVolumeDbspl(
        const CsvHeaderRow& headerRow, const std::vector<CsvCell>& row, const unsigned columnIndex, const unsigned rowIndex, uint16_t& value)
    {
        const auto& cell = row[columnIndex];

        float dbsplValue;
        if (!cell.AsFloat(dbsplValue))
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - Must be a float", rowIndex + 1, colName);
            return false;
        }

        if (dbsplValue < 0.0f || dbsplValue > 100.0f)
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - {} [0.0, 100.0]", rowIndex + 1, colName, dbsplValue);
            return false;
        }

        value = static_cast<uint16_t>(Common::DbsplToLinear(dbsplValue) * static_cast<float>(std::numeric_limits<uint16_t>::max()));

        return true;
    }

    bool ReadColumnPitchCents(
        const CsvHeaderRow& headerRow, const std::vector<CsvCell>& row, const unsigned columnIndex, const unsigned rowIndex, uint16_t& value)
    {
        const auto& cell = row[columnIndex];

        float centsValue;
        if (!cell.AsFloat(centsValue))
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - Must be a float", rowIndex + 1, colName);
            return false;
        }

        if (centsValue < -2400.0f || centsValue > 1200.0f)
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - {} [-2400.0, 1200.0]", rowIndex + 1, colName, centsValue);
            return false;
        }

        value = static_cast<uint16_t>(Common::CentsToHertz(centsValue) * static_cast<float>(std::numeric_limits<int16_t>::max()));

        return true;
    }

    bool ReadColumnUInt8(const CsvHeaderRow& headerRow,
                         const std::vector<CsvCell>& row,
                         const unsigned columnIndex,
                         const unsigned rowIndex,
                         uint8_t& value,
                         uint8_t min = std::numeric_limits<uint8_t>::min(),
                         uint8_t max = std::numeric_limits<uint8_t>::max())
    {
        const auto& cell = row[columnIndex];

        uint32_t value32;
        if (!cell.AsUInt32(value32))
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - Must be a uint", rowIndex + 1, colName);
            return false;
        }

        if (value32 < min || value32 > max)
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - {} [{}, {}]", rowIndex + 1, colName, value32, min, max);
            return false;
        }

        value = static_cast<uint8_t>(value32);

        return true;
    }

    bool ReadColumnInt16(const CsvHeaderRow& headerRow,
                         const std::vector<CsvCell>& row,
                         const unsigned columnIndex,
                         const unsigned rowIndex,
                         int16_t& value,
                         int16_t min = std::numeric_limits<int16_t>::min(),
                         int16_t max = std::numeric_limits<int16_t>::max())
    {
        const auto& cell = row[columnIndex];

        int32_t value32;
        if (!cell.AsInt32(value32))
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - Must be a int", rowIndex + 1, colName);
            return false;
        }

        if (value32 < min || value32 > max)
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - {} [{}, {}]", rowIndex + 1, colName, value32, min, max);
            return false;
        }

        value = static_cast<int16_t>(value32);

        return true;
    }

    bool ReadColumnUInt16(const CsvHeaderRow& headerRow,
                          const std::vector<CsvCell>& row,
                          const unsigned columnIndex,
                          const unsigned rowIndex,
                          uint16_t& value,
                          uint16_t min = std::numeric_limits<uint16_t>::min(),
                          uint16_t max = std::numeric_limits<uint16_t>::max())
    {
        const auto& cell = row[columnIndex];

        uint32_t value32;
        if (!cell.AsUInt32(value32))
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - Must be a uint", rowIndex + 1, colName);
            return false;
        }

        if (value32 < min || value32 > max)
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - {} [{}, {}]", rowIndex + 1, colName, value32, min, max);
            return false;
        }

        value = static_cast<uint16_t>(value32);

        return true;
    }

    bool ReadColumnNormByte(const CsvHeaderRow& headerRow, const std::vector<CsvCell>& row, const unsigned columnIndex, const unsigned rowIndex, uint8_t& value)
    {
        const auto& cell = row[columnIndex];

        float valueFloat;
        if (!cell.AsFloat(valueFloat))
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - Must be a float", rowIndex + 1, colName);
            return false;
        }

        if (valueFloat < 0.0f || valueFloat > 1.0f)
        {
            const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
            con::error("Invalid value for row {} col '{}' - {} [0.0, 1.0]", rowIndex + 1, colName, valueFloat);
            return false;
        }

        value = static_cast<uint8_t>(valueFloat * static_cast<float>(std::numeric_limits<uint8_t>::max()));

        return true;
    }

    bool ReadColumnHash(const std::vector<CsvCell>& row, const unsigned columnIndex, unsigned& value)
    {
        const auto& cell = row[columnIndex];

        if (cell.m_value.empty())
        {
            value = 0u;
            return true;
        }

        if (cell.m_value[0] == '@')
            value = std::strtoul(&cell.m_value[1], nullptr, 16);
        else
            value = Common::SND_HashName(cell.m_value.c_str());

        return true;
    }

    bool ReadColumnEnum_(const CsvHeaderRow& headerRow,
                         const std::vector<CsvCell>& row,
                         const unsigned columnIndex,
                         const unsigned rowIndex,
                         uint8_t& value,
                         const char* const* enumValues,
                         const size_t enumValueCount)
    {
        const auto& cell = row[columnIndex];

        assert(enumValueCount <= std::numeric_limits<uint8_t>::max());
        for (auto i = 0u; i < enumValueCount; i++)
        {
            if (cell.m_value == enumValues[i])
            {
                value = static_cast<uint8_t>(i);
                return true;
            }
        }

        const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
        con::error("Invalid value for row {} col '{}' - {}. Must be one of:", rowIndex + 1, colName, cell.m_value);
        for (auto i = 0u; i < enumValueCount; i++)
            con::error("  {}", enumValues[i]);

        return false;
    }

    template<size_t Size>
    bool ReadColumnEnum(const CsvHeaderRow& headerRow,
                        const std::vector<CsvCell>& row,
                        const unsigned columnIndex,
                        const unsigned rowIndex,
                        uint8_t& value,
                        const char* const (&enumValues)[Size])
    {
        return ReadColumnEnum_(headerRow, row, columnIndex, rowIndex, value, enumValues, Size);
    }

    bool ReadColumnEnumFlags_(const CsvHeaderRow& headerRow,
                              const std::vector<CsvCell>& row,
                              const unsigned columnIndex,
                              const unsigned rowIndex,
                              uint8_t& value,
                              const char* const* enumValues,
                              const size_t enumValueCount)
    {
        const auto& cell = row[columnIndex];
        const auto entries = utils::StringSplit(cell.m_value, ' ');

        assert(enumValueCount <= std::numeric_limits<uint8_t>::max());

        value = 0u;
        for (const auto& entry : entries)
        {
            if (entry.empty())
                continue;

            auto foundValue = false;
            for (auto i = 0u; i < enumValueCount; i++)
            {
                if (entry == enumValues[i])
                {
                    value |= static_cast<uint8_t>(1 << i);
                    foundValue = true;
                    break;
                }
            }

            if (!foundValue)
            {
                const auto& colName = headerRow.HeaderNameForColumn(columnIndex);
                con::error("Invalid value for row {} col '{}' - {}. Must be one of:", rowIndex + 1, colName, entry);
                for (auto i = 0u; i < enumValueCount; i++)
                    con::error("  {}", enumValues[i]);
                return false;
            }
        }

        return true;
    }

    template<size_t Size>
    bool ReadColumnEnumFlags(const CsvHeaderRow& headerRow,
                             const std::vector<CsvCell>& row,
                             const unsigned columnIndex,
                             const unsigned rowIndex,
                             uint8_t& value,
                             const char* const (&enumValues)[Size])
    {
        return ReadColumnEnumFlags_(headerRow, row, columnIndex, rowIndex, value, enumValues, Size);
    }

    class SoundAliasHeaders
    {
    public:
        SoundAliasHeaders() = default;

        bool From(const CsvHeaderRow& headerRow)
        {
            // clang-format off
            return headerRow.RequireIndexForHeader("Name", m_name)
                && headerRow.RequireIndexForHeader("FileSource", m_file_source)
                && headerRow.RequireIndexForHeader("Secondary", m_secondary)
                && headerRow.RequireIndexForHeader("Storage", m_storage)
                && headerRow.RequireIndexForHeader("Bus", m_bus)
                && headerRow.RequireIndexForHeader("VolumeGroup", m_volume_group)
                && headerRow.RequireIndexForHeader("DuckGroup", m_duck_group)
                && headerRow.RequireIndexForHeader("Duck", m_duck)
                && headerRow.RequireIndexForHeader("ReverbSend", m_reverb_send)
                && headerRow.RequireIndexForHeader("CenterSend", m_center_send)
                && headerRow.RequireIndexForHeader("VolMin", m_vol_min)
                && headerRow.RequireIndexForHeader("VolMax", m_vol_max)
                && headerRow.RequireIndexForHeader("DistMin", m_dist_min)
                && headerRow.RequireIndexForHeader("DistMaxDry", m_dist_max_dry)
                && headerRow.RequireIndexForHeader("DistMaxWet", m_dist_max_wet)
                && headerRow.RequireIndexForHeader("DryMinCurve", m_dry_min_curve)
                && headerRow.RequireIndexForHeader("DryMaxCurve", m_dry_max_curve)
                && headerRow.RequireIndexForHeader("WetMinCurve", m_wet_min_curve)
                && headerRow.RequireIndexForHeader("WetMaxCurve", m_wet_max_curve)
                && headerRow.RequireIndexForHeader("LimitCount", m_limit_count)
                && headerRow.RequireIndexForHeader("EntityLimitCount", m_entity_limit_count)
                && headerRow.RequireIndexForHeader("LimitType", m_limit_type)
                && headerRow.RequireIndexForHeader("EntityLimitType", m_entity_limit_type)
                && headerRow.RequireIndexForHeader("PitchMin", m_pitch_min)
                && headerRow.RequireIndexForHeader("PitchMax", m_pitch_max)
                && headerRow.RequireIndexForHeader("PriorityMin", m_priority_min)
                && headerRow.RequireIndexForHeader("PriorityMax", m_priority_max)
                && headerRow.RequireIndexForHeader("PriorityThresholdMin", m_priority_threshold_min)
                && headerRow.RequireIndexForHeader("PriorityThresholdMax", m_priority_threshold_max)
                && headerRow.RequireIndexForHeader("PanType", m_pan_type)
                && headerRow.RequireIndexForHeader("Pan", m_pan)
                && headerRow.RequireIndexForHeader("Looping", m_looping)
                && headerRow.RequireIndexForHeader("RandomizeType", m_randomize_type)
                && headerRow.RequireIndexForHeader("Probability", m_probability)
                && headerRow.RequireIndexForHeader("StartDelay", m_start_delay)
                && headerRow.RequireIndexForHeader("EnvelopMin", m_envelop_min)
                && headerRow.RequireIndexForHeader("EnvelopMax", m_envelop_max)
                && headerRow.RequireIndexForHeader("EnvelopPercent", m_envelop_percentage)
                && headerRow.RequireIndexForHeader("OcclusionLevel", m_occlusion_level)
                && headerRow.RequireIndexForHeader("IsBig", m_is_big)
                && headerRow.RequireIndexForHeader("DistanceLpf", m_distance_lpf)
                && headerRow.RequireIndexForHeader("FluxType", m_flux_type)
                && headerRow.RequireIndexForHeader("FluxTime", m_flux_time)
                && headerRow.RequireIndexForHeader("Subtitle", m_subtitle)
                && headerRow.RequireIndexForHeader("Doppler", m_doppler)
                && headerRow.RequireIndexForHeader("ContextType", m_context_type)
                && headerRow.RequireIndexForHeader("ContextValue", m_context_value)
                && headerRow.RequireIndexForHeader("Timescale", m_timescale)
                && headerRow.RequireIndexForHeader("IsMusic", m_is_music)
                && headerRow.RequireIndexForHeader("IsCinematic", m_is_cinematic)
                && headerRow.RequireIndexForHeader("FadeIn", m_fade_in)
                && headerRow.RequireIndexForHeader("FadeOut", m_fade_out)
                && headerRow.RequireIndexForHeader("Pauseable", m_pauseable)
                && headerRow.RequireIndexForHeader("StopOnEntDeath", m_stop_on_ent_death)
                && headerRow.RequireIndexForHeader("StopOnPlay", m_stop_on_play)
                && headerRow.RequireIndexForHeader("DopplerScale", m_doppler_scale)
                && headerRow.RequireIndexForHeader("FutzPatch", m_futz_patch)
                && headerRow.RequireIndexForHeader("VoiceLimit", m_voice_limit)
                && headerRow.RequireIndexForHeader("IgnoreMaxDist", m_ignore_max_dist)
                && headerRow.RequireIndexForHeader("NeverPlayTwice", m_never_play_twice);
            // clang-format on
        }

        unsigned m_name;
        unsigned m_file_source;
        unsigned m_secondary;
        unsigned m_storage;
        unsigned m_bus;
        unsigned m_volume_group;
        unsigned m_duck_group;
        unsigned m_duck;
        unsigned m_reverb_send;
        unsigned m_center_send;
        unsigned m_vol_min;
        unsigned m_vol_max;
        unsigned m_dist_min;
        unsigned m_dist_max_dry;
        unsigned m_dist_max_wet;
        unsigned m_dry_min_curve;
        unsigned m_dry_max_curve;
        unsigned m_wet_min_curve;
        unsigned m_wet_max_curve;
        unsigned m_limit_count;
        unsigned m_entity_limit_count;
        unsigned m_limit_type;
        unsigned m_entity_limit_type;
        unsigned m_pitch_min;
        unsigned m_pitch_max;
        unsigned m_priority_min;
        unsigned m_priority_max;
        unsigned m_priority_threshold_min;
        unsigned m_priority_threshold_max;
        unsigned m_pan_type;
        unsigned m_pan;
        unsigned m_looping;
        unsigned m_randomize_type;
        unsigned m_probability;
        unsigned m_start_delay;
        unsigned m_envelop_min;
        unsigned m_envelop_max;
        unsigned m_envelop_percentage;
        unsigned m_occlusion_level;
        unsigned m_is_big;
        unsigned m_distance_lpf;
        unsigned m_flux_type;
        unsigned m_flux_time;
        unsigned m_subtitle;
        unsigned m_doppler;
        unsigned m_context_type;
        unsigned m_context_value;
        unsigned m_timescale;
        unsigned m_is_music;
        unsigned m_is_cinematic;
        unsigned m_fade_in;
        unsigned m_fade_out;
        unsigned m_pauseable;
        unsigned m_stop_on_ent_death;
        unsigned m_stop_on_play;
        unsigned m_doppler_scale;
        unsigned m_futz_patch;
        unsigned m_voice_limit;
        unsigned m_ignore_max_dist;
        unsigned m_never_play_twice;
    };

    bool LoadSoundAlias(SndAlias& alias,
                        const std::vector<CsvCell>& row,
                        const unsigned int rowIndex,
                        const CsvHeaderRow& headerRow,
                        const SoundAliasHeaders& headers,
                        MemoryManager& memory)
    {
        memset(&alias, 0, sizeof(SndAlias));

        const auto& name = row[headers.m_name];
        if (name.m_value.empty())
            return false;

        alias.name = memory.Dup(name.m_value.c_str());
        alias.id = Common::SND_HashName(alias.name);

        const auto& aliasFileName = row[headers.m_file_source];
        if (!aliasFileName.m_value.empty())
        {

            alias.assetFileName = memory.Dup(aliasFileName.m_value.c_str());
            alias.assetId = Common::SND_HashName(alias.assetFileName);
        }

        uint8_t dryMinCurve = 0u, dryMaxCurve = 0u, wetMinCurve = 0u, wetMaxCurve = 0u, limitType = 0u, entityLimitType = 0u, randomizeType = 0u, fluxType = 0u,
                storage = 0u, volumeGroup = 0u, distanceLpf = 0u, doppler = 0u, isBig = 0u, looping = 0u, panType = 0u, isMusic = 0u, timescale = 0u,
                pauseable = 0u, stopOnEntDeath = 0u, busType = 0u, voiceLimit = 0u, ignoreMaxDist = 0u, neverPlayTwice = 0u, isCinematic = 0u;
        // clang-format off
        const auto couldReadSoundAlias = 
               ReadColumnString(row, headers.m_secondary, alias.secondaryName, memory)
            && ReadColumnEnum(headerRow, row, headers.m_storage, rowIndex, storage, SOUND_LOAD_TYPES)
            && ReadColumnEnum(headerRow, row, headers.m_bus, rowIndex, busType, SOUND_BUS_IDS)
            && ReadColumnEnum(headerRow, row, headers.m_volume_group, rowIndex, volumeGroup, SOUND_GROUPS)
            && ReadColumnEnum(headerRow, row, headers.m_duck_group, rowIndex, alias.duckGroup, SOUND_DUCK_GROUPS)
            && ReadColumnHash(row, headers.m_duck, alias.duck)
            && ReadColumnVolumeDbspl(headerRow, row, headers.m_reverb_send, rowIndex, alias.reverbSend)
            && ReadColumnVolumeDbspl(headerRow, row, headers.m_center_send, rowIndex, alias.centerSend)
            && ReadColumnVolumeDbspl(headerRow, row, headers.m_vol_min, rowIndex, alias.volMin)
            && ReadColumnVolumeDbspl(headerRow, row, headers.m_vol_max, rowIndex, alias.volMax)
            && ReadColumnUInt16(headerRow, row, headers.m_dist_min, rowIndex, alias.distMin)
            && ReadColumnUInt16(headerRow, row, headers.m_dist_max_dry, rowIndex, alias.distMax)
            && ReadColumnUInt16(headerRow, row, headers.m_dist_max_wet, rowIndex, alias.distReverbMax)
            && ReadColumnEnum(headerRow, row, headers.m_dry_min_curve, rowIndex, dryMinCurve, SOUND_CURVES)
            && ReadColumnEnum(headerRow, row, headers.m_dry_max_curve, rowIndex, dryMaxCurve, SOUND_CURVES)
            && ReadColumnEnum(headerRow, row, headers.m_wet_min_curve, rowIndex, wetMinCurve, SOUND_CURVES)
            && ReadColumnEnum(headerRow, row, headers.m_wet_max_curve, rowIndex, wetMaxCurve, SOUND_CURVES)
            && ReadColumnUInt8(headerRow, row, headers.m_limit_count, rowIndex, alias.limitCount, 0u, 128u)
            && ReadColumnUInt8(headerRow, row, headers.m_entity_limit_count, rowIndex, alias.entityLimitCount, 0u, 128u)
            && ReadColumnEnum(headerRow, row, headers.m_limit_type, rowIndex, limitType, SOUND_LIMIT_TYPES)
            && ReadColumnEnum(headerRow, row, headers.m_entity_limit_type, rowIndex, entityLimitType, SOUND_LIMIT_TYPES)
            && ReadColumnPitchCents(headerRow, row, headers.m_pitch_min, rowIndex, alias.pitchMin)
            && ReadColumnPitchCents(headerRow, row, headers.m_pitch_max, rowIndex, alias.pitchMax)
            && ReadColumnUInt8(headerRow, row, headers.m_priority_min, rowIndex, alias.minPriority, 0u, 128u)
            && ReadColumnUInt8(headerRow, row, headers.m_priority_max, rowIndex, alias.maxPriority, 0u, 128u)
            && ReadColumnNormByte(headerRow, row, headers.m_priority_threshold_min, rowIndex, alias.minPriorityThreshold)
            && ReadColumnNormByte(headerRow, row, headers.m_priority_threshold_max, rowIndex, alias.maxPriorityThreshold)
            && ReadColumnEnum(headerRow, row, headers.m_pan_type, rowIndex, panType, SOUND_PAN_TYPES)
            && ReadColumnEnum(headerRow, row, headers.m_pan, rowIndex, alias.pan, SOUND_PANS)
            && ReadColumnEnum(headerRow, row, headers.m_looping, rowIndex, looping, SOUND_LOOP_TYPES)
            && ReadColumnEnumFlags(headerRow, row, headers.m_randomize_type, rowIndex, randomizeType, SOUND_RANDOMIZE_TYPES)
            && ReadColumnNormByte(headerRow, row, headers.m_probability, rowIndex, alias.probability)
            && ReadColumnUInt16(headerRow, row, headers.m_start_delay, rowIndex, alias.startDelay)
            && ReadColumnUInt16(headerRow, row, headers.m_envelop_min, rowIndex, alias.envelopMin)
            && ReadColumnUInt16(headerRow, row, headers.m_envelop_max, rowIndex, alias.envelopMax)
            && ReadColumnVolumeDbspl(headerRow, row, headers.m_envelop_percentage, rowIndex, alias.envelopPercentage)
            && ReadColumnNormByte(headerRow, row, headers.m_occlusion_level, rowIndex, alias.occlusionLevel)
            && ReadColumnEnum(headerRow, row, headers.m_is_big, rowIndex, isBig, SOUND_NO_YES)
            && ReadColumnEnum(headerRow, row, headers.m_distance_lpf, rowIndex, distanceLpf, SOUND_NO_YES)
            && ReadColumnEnum(headerRow, row, headers.m_flux_type, rowIndex, fluxType, SOUND_MOVE_TYPES)
            && ReadColumnUInt16(headerRow, row, headers.m_flux_time, rowIndex, alias.fluxTime)
            && ReadColumnString(row, headers.m_subtitle, alias.subtitle, memory)
            && ReadColumnEnum(headerRow, row, headers.m_doppler, rowIndex, doppler, SOUND_NO_YES)
            && ReadColumnHash(row, headers.m_context_type, alias.contextType)
            && ReadColumnHash(row, headers.m_context_value, alias.contextValue)
            && ReadColumnEnum(headerRow, row, headers.m_timescale, rowIndex, timescale, SOUND_NO_YES)
            && ReadColumnEnum(headerRow, row, headers.m_is_music, rowIndex, isMusic, SOUND_NO_YES)
            && ReadColumnEnum(headerRow, row, headers.m_is_cinematic, rowIndex, isCinematic, SOUND_NO_YES)
            && ReadColumnInt16(headerRow, row, headers.m_fade_in, rowIndex, alias.fadeIn, 0)
            && ReadColumnInt16(headerRow, row, headers.m_fade_out, rowIndex, alias.fadeOut, 0)
            && ReadColumnEnum(headerRow, row, headers.m_pauseable, rowIndex, pauseable, SOUND_NO_YES)
            && ReadColumnEnum(headerRow, row, headers.m_stop_on_ent_death, rowIndex, stopOnEntDeath, SOUND_NO_YES)
            && ReadColumnHash(row, headers.m_stop_on_play, alias.stopOnPlay)
            && ReadColumnInt16(headerRow, row, headers.m_doppler_scale, rowIndex, alias.dopplerScale, -100, 100)
            && ReadColumnHash(row, headers.m_futz_patch, alias.futzPatch)
            && ReadColumnEnum(headerRow, row, headers.m_voice_limit, rowIndex, voiceLimit, SOUND_NO_YES)
            && ReadColumnEnum(headerRow, row, headers.m_ignore_max_dist, rowIndex, ignoreMaxDist, SOUND_NO_YES)
            && ReadColumnEnum(headerRow, row, headers.m_never_play_twice, rowIndex, neverPlayTwice, SOUND_NO_YES)  
        ;
        // clang-format on

        if (!couldReadSoundAlias)
            return false;

        alias.flags.volumeMinFalloffCurve = dryMinCurve;
        alias.flags.volumeFalloffCurve = dryMaxCurve;
        alias.flags.reverbMinFalloffCurve = wetMinCurve;
        alias.flags.reverbFalloffCurve = wetMaxCurve;
        alias.flags.limitType = limitType;
        alias.flags.entityLimitType = entityLimitType;
        alias.flags.randomizeType = randomizeType;
        alias.flags.fluxType = fluxType;
        alias.flags.loadType = storage;
        alias.flags.volumeGroup = volumeGroup;
        alias.flags.distanceLpf = distanceLpf;
        alias.flags.doppler = doppler;
        alias.flags.isBig = isBig;
        alias.flags.looping = looping;
        alias.flags.panType = panType;
        alias.flags.isMusic = isMusic;
        alias.flags.timescale = timescale;
        alias.flags.pauseable = pauseable;
        alias.flags.stopOnEntDeath = stopOnEntDeath;
        alias.flags.busType = busType;
        alias.flags.voiceLimit = voiceLimit;
        alias.flags.ignoreMaxDist = ignoreMaxDist;
        alias.flags.neverPlayTwice = neverPlayTwice;
        alias.flags.isCinematic = isCinematic;

        return true;
    }

    SndAliasList CreateAliasList(std::vector<SndAlias>& aliases, MemoryManager& memory)
    {
        SndAliasList aliasList;
        aliasList.sequence = 0;
        aliasList.count = static_cast<int>(aliases.size());
        if (aliasList.count > 0)
        {
            aliasList.head = memory.Alloc<SndAlias>(aliasList.count);
            memcpy(aliasList.head, aliases.data(), sizeof(SndAlias) * aliasList.count);

            aliasList.id = aliasList.head[0].id;
            aliasList.name = aliasList.head[0].name;
        }
        else
        {
            aliasList.id = 0u;
            aliasList.name = nullptr;
            aliasList.head = nullptr;
        }

        aliases.clear();

        return aliasList;
    }

    bool LoadSoundAliasList(MemoryManager& memory, SndBank* sndBank, const SearchPathOpenFile& file, unsigned& loadedEntryCount, unsigned& streamedEntryCount)
    {
        const CsvInputStream csv(*file.m_stream);
        CsvHeaderRow headerRow;
        SoundAliasHeaders headers;
        if (!headerRow.Read(csv) || !headers.From(headerRow))
        {
            con::error("Invalid headers for aliases of sound bank {}", sndBank->name);
            return false;
        }

        std::vector<SndAliasList> aliasLists;
        std::vector<SndAlias> aliasList;
        std::vector<CsvCell> row;
        unsigned rowIndex = 0u;
        while (csv.NextRow(row))
        {
            SndAlias alias;
            if (!LoadSoundAlias(alias, row, rowIndex++, headerRow, headers, memory))
                return false;

            if (alias.flags.loadType == SA_LOADED)
                loadedEntryCount++;
            else
                streamedEntryCount++;

            if (!aliasList.empty() && aliasList[0].id != alias.id)
            {
                aliasLists.emplace_back(CreateAliasList(aliasList, memory));
                aliasList.emplace_back(alias);
            }
            else
                aliasList.emplace_back(alias);
        }

        if (!aliasList.empty())
            aliasLists.emplace_back(CreateAliasList(aliasList, memory));

        sndBank->aliasCount = static_cast<unsigned>(aliasLists.size());
        if (sndBank->aliasCount)
        {
            sndBank->alias = memory.Alloc<SndAliasList>(sndBank->aliasCount);
            memcpy(sndBank->alias, aliasLists.data(), sizeof(SndAliasList) * sndBank->aliasCount);
        }

        return true;
    }

    bool LoadSoundAliasIndexList(MemoryManager& memory, SndBank* sndBank)
    {
        // contains a list of all the alias ids in the sound bank
        sndBank->aliasIndex = memory.Alloc<SndIndexEntry>(sndBank->aliasCount);
        memset(sndBank->aliasIndex, 0xFF, sizeof(SndIndexEntry) * sndBank->aliasCount);

        const auto setAliasIndexList = std::make_unique<bool[]>(sndBank->aliasCount);

        for (auto i = 0u; i < sndBank->aliasCount; i++)
        {
            const auto idx = sndBank->alias[i].id % sndBank->aliasCount;
            if (sndBank->aliasIndex[idx].value == std::numeric_limits<unsigned short>::max())
            {
                sndBank->aliasIndex[idx].value = static_cast<unsigned short>(i);
                sndBank->aliasIndex[idx].next = std::numeric_limits<unsigned short>::max();
                setAliasIndexList[i] = true;
            }
        }

        for (auto i = 0u; i < sndBank->aliasCount; i++)
        {
            if (setAliasIndexList[i])
                continue;

            auto idx = sndBank->alias[i].id % sndBank->aliasCount;
            while (sndBank->aliasIndex[idx].next != std::numeric_limits<unsigned short>::max())
            {
                idx = sndBank->aliasIndex[idx].next;
            }

            auto offset = 1u;
            unsigned short freeIdx;
            while (true)
            {
                freeIdx = static_cast<unsigned short>((idx + offset) % sndBank->aliasCount);
                if (sndBank->aliasIndex[freeIdx].value == std::numeric_limits<unsigned short>::max())
                    break;

                freeIdx = static_cast<unsigned short>((idx + sndBank->aliasCount - offset) % sndBank->aliasCount);
                if (sndBank->aliasIndex[freeIdx].value == std::numeric_limits<unsigned short>::max())
                    break;

                offset++;
                freeIdx = std::numeric_limits<unsigned short>::max();

                if (offset >= sndBank->aliasCount)
                    break;
            }

            if (freeIdx == std::numeric_limits<unsigned short>::max())
            {
                con::error("Unable to allocate sound bank alias index list");
                return false;
            }

            sndBank->aliasIndex[idx].next = freeIdx;
            sndBank->aliasIndex[freeIdx].value = static_cast<unsigned short>(i);
            sndBank->aliasIndex[freeIdx].next = std::numeric_limits<unsigned short>::max();
            setAliasIndexList[i] = true;
        }

        return true;
    }

    class RadverbHeaders
    {
    public:
        RadverbHeaders() = default;

        bool From(const CsvHeaderRow& headerRow)
        {
            // clang-format off
            return headerRow.RequireIndexForHeader("name", m_name)
                && headerRow.RequireIndexForHeader("smoothing", m_smoothing)
                && headerRow.RequireIndexForHeader("earlyTime", m_early_time)
                && headerRow.RequireIndexForHeader("lateTime", m_late_time)
                && headerRow.RequireIndexForHeader("earlyGain", m_early_gain)
                && headerRow.RequireIndexForHeader("lateGain", m_late_gain)
                && headerRow.RequireIndexForHeader("returnGain", m_return_gain) 
                && headerRow.RequireIndexForHeader("earlyLpf", m_early_lpf)
                && headerRow.RequireIndexForHeader("lateLpf", m_late_lpf) 
                && headerRow.RequireIndexForHeader("inputLpf", m_input_lpf)
                && headerRow.RequireIndexForHeader("dampLpf", m_damp_lpf) 
                && headerRow.RequireIndexForHeader("wallReflect", m_wall_reflect)
                && headerRow.RequireIndexForHeader("dryGain", m_dry_gain) 
                && headerRow.RequireIndexForHeader("earlySize", m_early_size)
                && headerRow.RequireIndexForHeader("lateSize", m_late_size) 
                && headerRow.RequireIndexForHeader("diffusion", m_diffusion)
                && headerRow.RequireIndexForHeader("returnHighpass", m_return_highpass);
            // clang-format on
        }

        unsigned m_name;
        unsigned m_smoothing;
        unsigned m_early_time;
        unsigned m_late_time;
        unsigned m_early_gain;
        unsigned m_late_gain;
        unsigned m_return_gain;
        unsigned m_early_lpf;
        unsigned m_late_lpf;
        unsigned m_input_lpf;
        unsigned m_damp_lpf;
        unsigned m_wall_reflect;
        unsigned m_dry_gain;
        unsigned m_early_size;
        unsigned m_late_size;
        unsigned m_diffusion;
        unsigned m_return_highpass;
    };

    bool LoadSoundRadverbs(MemoryManager& memory, SndBank* sndBank, const SearchPathOpenFile& file)
    {
        const CsvInputStream csv(*file.m_stream);
        CsvHeaderRow csvHeaders;
        RadverbHeaders headers;
        if (!csvHeaders.Read(csv) || !headers.From(csvHeaders))
        {
            con::error("Invalid headers for radverbs of sound bank {}", sndBank->name);
            return false;
        }

        std::vector<SndRadverb> radverbs;
        std::vector<CsvCell> row;
        while (csv.NextRow(row))
        {
            const auto& name = row[headers.m_name];
            if (name.m_value.empty())
                return false;

            SndRadverb radverb;
            strncpy(radverb.name, name.m_value.c_str(), 32);
            radverb.id = Common::SND_HashName(name.m_value.c_str());

            // clang-format off
            const auto readRadverb = 
                   row[headers.m_smoothing].AsFloat(radverb.smoothing)
                && row[headers.m_early_time].AsFloat(radverb.earlyTime)
                && row[headers.m_late_time].AsFloat(radverb.lateTime)
                && row[headers.m_early_gain].AsFloat(radverb.earlyGain)
                && row[headers.m_late_gain].AsFloat(radverb.lateGain)
                && row[headers.m_return_gain].AsFloat(radverb.returnGain)
                && row[headers.m_early_lpf].AsFloat(radverb.earlyLpf)
                && row[headers.m_late_lpf].AsFloat(radverb.lateLpf)
                && row[headers.m_input_lpf].AsFloat(radverb.inputLpf)
                && row[headers.m_damp_lpf].AsFloat(radverb.dampLpf)
                && row[headers.m_wall_reflect].AsFloat(radverb.wallReflect)
                && row[headers.m_dry_gain].AsFloat(radverb.dryGain)
                && row[headers.m_early_size].AsFloat(radverb.earlySize)
                && row[headers.m_late_size].AsFloat(radverb.lateSize)
                && row[headers.m_diffusion].AsFloat(radverb.diffusion)
                && row[headers.m_return_highpass].AsFloat(radverb.returnHighpass);
            // clang-format on

            if (!readRadverb)
                return false;

            radverbs.emplace_back(radverb);
        }

        sndBank->radverbCount = static_cast<unsigned>(radverbs.size());
        if (sndBank->radverbCount)
        {
            sndBank->radverbs = memory.Alloc<SndRadverb>(sndBank->radverbCount);
            memcpy(sndBank->radverbs, radverbs.data(), sizeof(SndRadverb) * sndBank->radverbCount);
        }

        return true;
    }

    bool LoadSoundDuckList(ISearchPath& searchPath, MemoryManager& memory, SndBank* sndBank, const SearchPathOpenFile& file)
    {
        const CsvInputStream csv(*file.m_stream);
        CsvHeaderRow headerRow;
        headerRow.Read(csv);
        const auto nameRow = headerRow.GetIndexForHeader("name");
        if (!nameRow)
        {
            con::error("Missing name header for ducks of sound bank {}", sndBank->name);
            return false;
        }

        std::vector<SndDuck> ducks;
        std::vector<std::string> row;
        while (csv.NextRow(row))
        {
            const auto& name = row[*nameRow];
            if (name.empty())
                return false;

            const auto duckFile = searchPath.Open(std::format("soundbank/ducks/{}.duk", name));
            if (!duckFile.IsOpen())
            {
                con::error("Unable to find .duk file for {} in ducklist for sound bank {}", name, sndBank->name);
                return false;
            }

            SndDuck duck;
            strncpy(duck.name, name.data(), 32);
            duck.id = Common::SND_HashName(name.data());

            auto duckJson = nlohmann::json::parse(*duckFile.m_stream);
            duck.fadeIn = duckJson["fadeIn"].get<float>();
            duck.fadeOut = duckJson["fadeOut"].get<float>();
            duck.startDelay = duckJson["startDelay"].get<float>();
            duck.distance = duckJson["distance"].get<float>();
            duck.length = duckJson["length"].get<float>();
            duck.updateWhilePaused = duckJson["updateWhilePaused"].get<int>();

            duck.fadeInCurve = duckJson["fadeInCurveId"].get<unsigned int>();
            duck.fadeOutCurve = duckJson["fadeOutCurveId"].get<unsigned int>();

            if (duckJson.contains("fadeInCurve"))
                duck.fadeInCurve = Common::SND_HashName(duckJson["fadeInCurve"].get<std::string>().data());

            if (duckJson.contains("fadeOutCurve"))
                duck.fadeOutCurve = Common::SND_HashName(duckJson["fadeOutCurve"].get<std::string>().data());

            duck.attenuation = memory.Alloc<SndFloatAlign16>(32u);
            duck.filter = memory.Alloc<SndFloatAlign16>(32u);

            for (auto& valueJson : duckJson["values"])
            {
                const auto index = GetValueIndex(valueJson["duckGroup"].get<std::string>(), SOUND_DUCK_GROUPS, std::extent_v<decltype(SOUND_DUCK_GROUPS)>);

                duck.attenuation[index] = valueJson["attenuation"].get<float>();
                duck.filter[index] = valueJson["filter"].get<float>();
            }

            ducks.emplace_back(duck);
        }

        sndBank->duckCount = static_cast<unsigned>(ducks.size());
        if (sndBank->duckCount)
        {
            sndBank->ducks = memory.Alloc<SndDuck>(sndBank->duckCount);
            memcpy(sndBank->ducks, ducks.data(), sizeof(SndDuck) * sndBank->duckCount);
        }

        return true;
    }

    class SoundBankLoader final : public AssetCreator<AssetSoundBank>
    {
    public:
        SoundBankLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            if (assetName.find('.') == std::string::npos)
            {
                con::error("A language must be specific in the sound bank asset name! (Ex: mpl_common.all)");
                return AssetCreationResult::Failure();
            }

            const auto aliasFile = m_search_path.Open(std::format("soundbank/{}.aliases.csv", assetName));
            if (!aliasFile.IsOpen())
                return AssetCreationResult::NoAction();

            auto* sndBank = m_memory.Alloc<SndBank>();
            sndBank->name = m_memory.Dup(assetName.c_str());

            const auto sndBankLocalization = utils::StringSplit(assetName, '.');

            unsigned loadedEntryCount = 0u, streamedEntryCount = 0u;
            if (!LoadSoundAliasList(m_memory, sndBank, aliasFile, loadedEntryCount, streamedEntryCount))
                return AssetCreationResult::Failure();

            if (!LoadSoundAliasIndexList(m_memory, sndBank))
                return AssetCreationResult::Failure();

            const auto radverbFile = m_search_path.Open(std::format("soundbank/{}.reverbs.csv", assetName));
            if (radverbFile.IsOpen())
            {
                if (!LoadSoundRadverbs(m_memory, sndBank, radverbFile))
                {
                    con::error("Sound bank reverbs file for {} is invalid", assetName);
                    return AssetCreationResult::Failure();
                }
            }

            const auto duckListFile = m_search_path.Open(std::format("soundbank/{}.ducklist.csv", assetName));
            if (duckListFile.IsOpen())
            {
                if (!LoadSoundDuckList(m_search_path, m_memory, sndBank, duckListFile))
                {
                    con::error("Sound bank ducklist file for {} is invalid", assetName);
                    return AssetCreationResult::Failure();
                }
            }

            std::unique_ptr<std::ofstream> sablStream, sabsStream;
            std::unique_ptr<SoundBankWriter> sablWriter, sabsWriter;

            if (loadedEntryCount > 0)
            {
                sndBank->loadAssetBank.zone = m_memory.Dup(sndBankLocalization.at(0).c_str());
                sndBank->loadAssetBank.language = m_memory.Dup(sndBankLocalization.at(1).c_str());
                memset(sndBank->loadAssetBank.linkTimeChecksum, 0xCC, 16);

                sndBank->loadedAssets.loadedCount = 0;
                sndBank->loadedAssets.zone = m_memory.Dup(sndBankLocalization.at(0).c_str());
                sndBank->loadedAssets.language = m_memory.Dup(sndBankLocalization.at(1).c_str());
                sndBank->loadedAssets.entryCount = loadedEntryCount;
                sndBank->loadedAssets.entries = m_memory.Alloc<SndAssetBankEntry>(loadedEntryCount);

                sndBank->runtimeAssetLoad = true;

                const auto sablName = std::format("{}.sabl", assetName);
                sablStream = OpenSoundBankOutputFile(sablName);
                if (sablStream)
                    sablWriter = SoundBankWriter::Create(sablName, *sablStream, m_search_path);
            }

            if (streamedEntryCount > 0)
            {
                sndBank->streamAssetBank.zone = m_memory.Dup(sndBankLocalization.at(0).c_str());
                sndBank->streamAssetBank.language = m_memory.Dup(sndBankLocalization.at(1).c_str());
                memset(sndBank->streamAssetBank.linkTimeChecksum, 0xCC, 16);

                const auto sabsName = std::format("{}.sabs", assetName);
                sabsStream = OpenSoundBankOutputFile(sabsName);
                if (sabsStream)
                    sabsWriter = SoundBankWriter::Create(sabsName, *sabsStream, m_search_path);
            }

            // add aliases to the correct sound bank writer
            for (auto i = 0u; i < sndBank->aliasCount; i++)
            {
                const auto* aliasList = &sndBank->alias[i];
                for (auto j = 0; j < aliasList->count; j++)
                {
                    const auto* alias = &aliasList->head[j];

                    if (alias->assetFileName && alias->assetId)
                    {
                        if (sabsWriter && alias->flags.loadType == SA_STREAMED)
                            sabsWriter->AddSound(GetSoundFilePath(alias), alias->assetId, alias->flags.looping, true);
                        else if (sablWriter)
                            sablWriter->AddSound(GetSoundFilePath(alias), alias->assetId, alias->flags.looping);
                    }
                }
            }

            // write the output linked sound bank
            if (sablWriter)
            {
                size_t dataSize = 0u;
                const auto result = sablWriter->Write(dataSize);
                sablStream->close();

                if (result)
                {
                    sndBank->loadedAssets.dataSize = static_cast<unsigned>(dataSize);
                    sndBank->loadedAssets.data = m_memory.Alloc<SndChar2048>(dataSize);
                }
                else
                {
                    con::error("Loaded sound bank for {} failed to generate. Please check your build files.", assetName);
                    return AssetCreationResult::Failure();
                }
            }

            // write the output streamed sound bank
            if (sabsWriter)
            {
                size_t dataSize = 0u;
                const auto result = sabsWriter->Write(dataSize);
                sabsStream->close();

                if (!result)
                {
                    con::error("Streamed sound bank for {} failed to generate. Please check your build files.", assetName);
                    return AssetCreationResult::Failure();
                }
            }

            return AssetCreationResult::Success(context.AddAsset<AssetSoundBank>(assetName, sndBank));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace sound
{
    std::unique_ptr<AssetCreator<AssetSoundBank>> CreateSoundBankLoaderT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<SoundBankLoader>(memory, searchPath);
    }
} // namespace sound
