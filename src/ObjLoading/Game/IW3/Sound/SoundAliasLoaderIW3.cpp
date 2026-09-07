#include "SoundAliasLoaderIW3.h"

#include "Csv/CsvStream.h"
#include "Game/IW3/SoundConstantsIW3.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <format>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

using namespace IW3;

namespace
{
    struct AliasRow
    {
        std::array<std::string, SA_NUMFIELDS> fields;
        std::string source;
        size_t line;
    };

    std::string Lower(std::string value)
    {
        utils::MakeStringLowerCase(value);
        return value;
    }

    float ParseFloat(const std::string& value)
    {
        float result;
        if (!CsvCell(value).AsFloat(result) || !std::isfinite(result))
            throw std::runtime_error(std::format("Invalid number '{}'", value));
        return result;
    }

    int ParseInt(const std::string& value)
    {
        int result;
        const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), result);
        if (error != std::errc() || end != value.data() + value.size())
            throw std::runtime_error(std::format("Invalid integer '{}'", value));
        return result;
    }

    class SoundAliasLoader final : public AssetCreator<AssetSound>
    {
    public:
        SoundAliasLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory),
              m_search_path(searchPath),
              m_zone(zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            try
            {
                // Explicit CSV imports register all contained aliases, as localized-string imports do.
                if (assetName.ends_with(".csv"))
                {
                    const auto fileName = std::format("soundaliases/{}", assetName);
                    if (!ReadFile(fileName))
                        return AssetCreationResult::NoAction();
                    auto result = AssetCreationResult::NoAction();
                    for (const auto& [name, rows] : m_rows)
                    {
                        if (std::ranges::any_of(rows,
                                                [&fileName](const AliasRow& row)
                                                {
                                                    return row.source == fileName;
                                                }))
                        {
                            const auto aliasResult = CreateAlias(name, context);
                            if (aliasResult.HasFailed())
                                return aliasResult;
                            if (aliasResult.HasBeenSuccessful())
                                result = aliasResult;
                        }
                    }
                    return result;
                }

                if (!m_indexed)
                {
                    std::set<std::string> files;
                    m_search_path.Find(SearchPathSearchOptions().FilterPrefix("soundaliases/").FilterExtensions("csv"),
                                       [&files](std::string name)
                                       {
                                           std::ranges::replace(name, '\\', '/');
                                           files.emplace(std::move(name));
                                       });
                    for (const auto& file : files)
                        ReadFile(file);
                    m_indexed = true;
                }
                return CreateAlias(Lower(assetName), context);
            }
            catch (const std::exception& e)
            {
                con::error("Failed to load IW3 sound alias '{}': {}", assetName, e.what());
                return AssetCreationResult::Failure();
            }
        }

    private:
        bool ReadFile(const std::string& fileName)
        {
            if (m_read_files.contains(fileName))
                return true;
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return false;
            CsvInputStream csv(*file.m_stream);
            std::vector<std::string> columns;
            std::vector<snd_alias_members_t> headers;
            size_t line = 0;
            std::vector<AliasRow> parsed;
            while (csv.NextRow(columns))
            {
                ++line;
                for (auto& value : columns)
                    utils::StringTrim(value);
                if (columns.empty() || columns[0].empty() || columns[0].starts_with('#') || columns[0].starts_with("//"))
                    continue;
                if (headers.empty())
                {
                    for (const auto& column : columns)
                    {
                        auto field = SA_INVALID;
                        for (auto i = SA_NAME; i < SA_NUMFIELDS; i = static_cast<snd_alias_members_t>(i + 1))
                            if (utils::StringEqualsIgnoreCase(column, g_pszSndAliasKeyNames[i]))
                                field = i;
                        if (field != SA_INVALID && std::ranges::find(headers, field) != headers.end())
                            throw std::runtime_error(std::format("{}:{}: duplicate header '{}'", fileName, line, column));
                        headers.push_back(field);
                    }
                    if (std::ranges::find(headers, SA_NAME) == headers.end() || std::ranges::find(headers, SA_FILE) == headers.end())
                        throw std::runtime_error(std::format("{}:{}: missing name/file headers", fileName, line));
                    continue;
                }
                AliasRow row{{}, fileName, line};
                for (size_t i = 0; i < std::min(columns.size(), headers.size()); i++)
                    if (headers[i] != SA_INVALID)
                        row.fields[headers[i]] = columns[i];
                if (row.fields[SA_NAME].empty() || row.fields[SA_FILE].empty())
                    throw std::runtime_error(std::format("{}:{}: missing alias name/file", fileName, line));
                row.fields[SA_NAME] = Lower(row.fields[SA_NAME]);
                parsed.push_back(std::move(row));
            }
            if (headers.empty())
                throw std::runtime_error(std::format("{}: missing CSV header", fileName));
            for (auto& row : parsed)
            {
                const auto name = row.fields[SA_NAME];
                m_rows[name].push_back(std::move(row));
            }
            m_read_files.emplace(fileName);
            return true;
        }

        float VolumeModifier(const std::string& name)
        {
            if (!m_volume_modifiers_loaded)
            {
                const auto file = m_search_path.Open("soundaliases/volumemodgroups.def");
                if (!file.IsOpen())
                    throw std::runtime_error("Missing soundaliases/volumemodgroups.def");
                std::string line;
                bool header = false;
                while (std::getline(*file.m_stream, line))
                {
                    std::istringstream tokens(line.substr(0, line.find("//")));
                    std::string key, value;
                    if (!(tokens >> key))
                        continue;
                    if (!header)
                    {
                        if (key != "VOLUMEMODGROUPS")
                            throw std::runtime_error("Invalid volumemodgroups.def header");
                        header = true;
                        continue;
                    }
                    if (!(tokens >> value))
                        throw std::runtime_error("Missing volume modifier value");
                    m_volume_modifiers[Lower(key)] = ParseFloat(value);
                }
                m_volume_modifiers_loaded = true;
            }
            const auto found = m_volume_modifiers.find(Lower(name));
            if (found == m_volume_modifiers.end())
                throw std::runtime_error(std::format("Unknown volume modifier '{}'", name));
            return found->second;
        }

        SndCurve* DefaultCurve(AssetCreationContext& context, AssetRegistration<AssetSound>& registration)
        {
            auto* info = m_zone.m_pools.GetAsset<AssetSoundCurve>("");
            if (!info)
            {
                auto* curve = m_memory.Alloc<SndCurve>();
                curve->filename = m_memory.Dup("");
                curve->knotCount = 2;
                curve->knots[0][1] = 1.0f;
                curve->knots[1][0] = 1.0f;
                info = context.AddAsset<AssetSoundCurve>("", curve);
            }
            registration.AddDependency(info);
            return info->Asset();
        }

        void BuildAlias(const AliasRow& row, snd_alias_t& alias, AssetCreationContext& context, AssetRegistration<AssetSound>& registration)
        {
            const auto& fields = row.fields;
            alias.aliasName = m_memory.Dup(fields[SA_NAME].c_str());
            alias.volMin = alias.volMax = alias.pitchMin = alias.pitchMax = alias.probability = alias.slavePercentage = 1.0f;
            alias.distMin = 120.0f;

            struct FloatField
            {
                snd_alias_members_t field;
                float snd_alias_t::* member;
                bool unitRange;
            };

            static constexpr FloatField FLOAT_FIELDS[]{
                {SA_VOL_MIN,           &snd_alias_t::volMin,            true },
                {SA_VOL_MAX,           &snd_alias_t::volMax,            true },
                {SA_PITCH_MIN,         &snd_alias_t::pitchMin,          false},
                {SA_PITCH_MAX,         &snd_alias_t::pitchMax,          false},
                {SA_DIST_MIN,          &snd_alias_t::distMin,           false},
                {SA_DIST_MAX,          &snd_alias_t::distMax,           false},
                {SA_PROBABILITY,       &snd_alias_t::probability,       false},
                {SA_LFEPERCENTAGE,     &snd_alias_t::lfePercentage,     true },
                {SA_CENTERPERCENTAGE,  &snd_alias_t::centerPercentage,  true },
                {SA_ENVELOPMIN,        &snd_alias_t::envelopMin,        false},
                {SA_ENVELOPMAX,        &snd_alias_t::envelopMax,        false},
                {SA_ENVELOPPERCENTAGE, &snd_alias_t::envelopPercentage, true },
            };
            for (const auto& field : FLOAT_FIELDS)
            {
                if (fields[field.field].empty())
                    continue;
                const auto value = ParseFloat(fields[field.field]);
                if (field.field == SA_PROBABILITY && value < 0.0f)
                    throw std::runtime_error("probability must be non-negative");
                if (field.unitRange && (value < 0.0f || value > 1.0f))
                    throw std::runtime_error(std::format("{} must be in [0, 1]", g_pszSndAliasKeyNames[field.field]));
                alias.*field.member = value;
            }
            if (fields[SA_VOL_MAX].empty())
                alias.volMax = alias.volMin;
            if (fields[SA_PITCH_MAX].empty())
                alias.pitchMax = alias.pitchMin;
            if (alias.volMin > alias.volMax)
                std::swap(alias.volMin, alias.volMax);
            if (alias.pitchMin > alias.pitchMax)
                std::swap(alias.pitchMin, alias.pitchMax);
            if (!fields[SA_VOL_MOD].empty())
            {
                const auto modifier = VolumeModifier(fields[SA_VOL_MOD]);
                alias.volMin = std::clamp(alias.volMin * modifier, 0.0f, 1.0f);
                alias.volMax = std::clamp(alias.volMax * modifier, 0.0f, 1.0f);
            }
            if (alias.distMax == 0.0f)
                alias.distMax = alias.distMin * 5.0f;
            if (alias.pitchMin <= 0.0f || alias.distMin <= 0.0f || alias.distMax <= alias.distMin || !std::isfinite(alias.distMax))
                throw std::runtime_error("Invalid pitch or distance range");
            if (!fields[SA_STARTDELAY].empty())
                alias.startDelay = ParseInt(fields[SA_STARTDELAY]);

            const auto typeName = Lower(fields[SA_TYPE]);
            const auto type = typeName.empty() || typeName == "loaded" ? SAT_LOADED : SAT_STREAMED;
            if (!typeName.empty() && typeName != "loaded" && typeName != "streamed")
                throw std::runtime_error(std::format("Unknown sound type '{}'", typeName));
            alias.flags = type << SND_ALIAS_FLAG_TYPE_SHIFT;
            if (!fields[SA_CHANNEL].empty())
            {
                const auto channel = Lower(fields[SA_CHANNEL]);
                const auto found = std::ranges::find(SOUND_CHANNEL_NAMES, channel);
                if (found == std::end(SOUND_CHANNEL_NAMES))
                    throw std::runtime_error(std::format("Unknown sound channel '{}'", channel));
                alias.flags |= static_cast<int>(found - std::begin(SOUND_CHANNEL_NAMES)) << SND_ALIAS_FLAG_CHANNEL_SHIFT;
            }
            const auto looping = Lower(fields[SA_LOOP]);
            if (looping == "looping" || looping == "rlooping")
                alias.flags |= SND_ALIAS_FLAG_LOOPING;
            else if (!looping.empty() && looping != "nonlooping")
                throw std::runtime_error(std::format("Unknown looping type '{}'", looping));
            if (looping == "rlooping")
                alias.flags |= SND_ALIAS_FLAG_RANDOM_LOOPING;
            if (!fields[SA_MASTERSLAVE].empty())
            {
                if (Lower(fields[SA_MASTERSLAVE]) == "master")
                    alias.flags |= SND_ALIAS_FLAG_MASTER;
                else
                {
                    alias.slavePercentage = ParseFloat(fields[SA_MASTERSLAVE]);
                    if (alias.slavePercentage < 0.0f || alias.slavePercentage > 1.0f)
                        throw std::runtime_error("Slave percentage must be in [0, 1]");
                    alias.flags |= SND_ALIAS_FLAG_SLAVE;
                }
            }
            std::istringstream reverb(Lower(fields[SA_REVERB]));
            std::string token;
            while (reverb >> token)
            {
                if (token == "fulldrylevel")
                    alias.flags |= SND_ALIAS_FLAG_FULL_DRY_LEVEL;
                else if (token == "nowetlevel")
                    alias.flags |= SND_ALIAS_FLAG_NO_WET_LEVEL;
                else
                    throw std::runtime_error(std::format("Unknown reverb flag '{}'", token));
            }
            if (!fields[SA_SUBTITLE].empty())
                alias.subtitle = m_memory.Dup(fields[SA_SUBTITLE].c_str());
            if (!fields[SA_SECONDARYALIASNAME].empty())
            {
                const auto name = Lower(fields[SA_SECONDARYALIASNAME]);
                alias.secondaryAliasName = m_memory.Dup(name.c_str());
                registration.AddIndirectAssetReference(IndirectAssetReference(ASSET_TYPE_SOUND, name));
            }
            if (!fields[SA_CHAINALIASNAME].empty())
            {
                const auto name = Lower(fields[SA_CHAINALIASNAME]);
                alias.chainAliasName = m_memory.Dup(name.c_str());
                registration.AddIndirectAssetReference(IndirectAssetReference(ASSET_TYPE_SOUND, name));
            }
            if (fields[SA_VOLUMEFALLOFFCURVE].empty())
                alias.volumeFalloffCurve = DefaultCurve(context, registration);
            else
            {
                auto* curve = context.LoadDependency<AssetSoundCurve>(fields[SA_VOLUMEFALLOFFCURVE]);
                if (!curve)
                    throw std::runtime_error("Could not load volume falloff curve");
                registration.AddDependency(curve);
                alias.volumeFalloffCurve = curve->Asset();
            }
            auto* speakerMap = context.LoadSubAsset<SubAssetSpeakerMap>(fields[SA_SPEAKERMAP]);
            if (!speakerMap)
                throw std::runtime_error("Could not load speaker map");
            alias.speakerMap = speakerMap->Asset();
            alias.soundFile = m_memory.Alloc<SoundFile>();
            alias.soundFile->type = static_cast<char>(type);
            alias.soundFile->exists = true;
            auto fileName = fields[SA_FILE];
            std::ranges::replace(fileName, '\\', '/');
            if (type == SAT_LOADED)
            {
                auto* sound = context.LoadDependency<AssetLoadedSound>(fileName);
                if (!sound)
                    throw std::runtime_error(std::format("Could not load sound '{}'", fileName));
                registration.AddDependency(sound);
                alias.soundFile->u.loadSnd = sound->Asset();
            }
            else
            {
                const auto slash = fileName.find_last_of('/');
                alias.soundFile->u.streamSnd.dir = m_memory.Dup(slash == std::string::npos ? "" : fileName.substr(0, slash).c_str());
                alias.soundFile->u.streamSnd.name = m_memory.Dup(slash == std::string::npos ? fileName.c_str() : fileName.substr(slash + 1).c_str());
            }
        }

        AssetCreationResult CreateAlias(const std::string& name, AssetCreationContext& context)
        {
            if (auto* existing = m_zone.m_pools.GetAsset<AssetSound>(name))
                return AssetCreationResult::Success(existing);
            const auto found = m_rows.find(name);
            if (found == m_rows.end())
                return AssetCreationResult::NoAction();
            // The stock linker sorts by name/sequence and lets the lexicographically later source override a sequence.
            std::map<int, const AliasRow*> sequences;
            for (const auto& row : found->second)
            {
                const auto sequence = row.fields[SA_SEQUENCE].empty() ? 0 : ParseInt(row.fields[SA_SEQUENCE]);
                const auto previous = sequences.find(sequence);
                if (previous != sequences.end())
                {
                    if (previous->second->source == row.source)
                        throw std::runtime_error(std::format("{}:{}: duplicate alias '{}' sequence {}", row.source, row.line, name, sequence));
                    if (previous->second->source > row.source)
                        continue;
                }
                sequences[sequence] = &row;
            }
            if (sequences.empty())
                return AssetCreationResult::NoAction();
            auto* list = m_memory.Alloc<snd_alias_list_t>();
            list->aliasName = m_memory.Dup(name.c_str());
            list->count = static_cast<int>(sequences.size());
            list->head = m_memory.Alloc<snd_alias_t>(sequences.size());
            AssetRegistration<AssetSound> registration(name, list);
            size_t index = 0;
            for (const auto& [sequence, row] : sequences)
            {
                try
                {
                    BuildAlias(*row, list->head[index++], context, registration);
                }
                catch (const std::exception& e)
                {
                    throw std::runtime_error(std::format("{}:{}: {}", row->source, row->line, e.what()));
                }
            }
            auto* info = context.AddAsset<AssetSound>(std::move(registration));
            // Publish the list first so secondary/chain alias cycles can resolve it.
            for (auto i = 0; i < list->count; i++)
            {
                if (list->head[i].secondaryAliasName)
                    context.LoadIndirectAssetReference<AssetSound>(list->head[i].secondaryAliasName);
                if (list->head[i].chainAliasName)
                    context.LoadIndirectAssetReference<AssetSound>(list->head[i].chainAliasName);
            }
            return AssetCreationResult::Success(info);
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        Zone& m_zone;
        bool m_indexed = false;
        bool m_volume_modifiers_loaded = false;
        std::set<std::string> m_read_files;
        std::map<std::string, std::vector<AliasRow>> m_rows;
        std::map<std::string, float> m_volume_modifiers;
    };
} // namespace

namespace sound_alias
{
    std::unique_ptr<AssetCreator<AssetSound>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<SoundAliasLoader>(memory, searchPath, zone);
    }
} // namespace sound_alias
