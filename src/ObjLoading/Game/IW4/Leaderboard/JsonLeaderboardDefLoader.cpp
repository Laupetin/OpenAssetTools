#include "JsonLeaderboardDefLoader.h"

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/Leaderboard/JsonLeaderboardDef.h"

#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace IW4;

namespace
{
    class JsonLoader
    {
    public:
        JsonLoader(std::istream& stream, MemoryManager& memory)
            : m_stream(stream),
              m_memory(memory)
        {
        }

        bool Load(LeaderboardDef& leaderboardDef) const
        {
            try
            {
                const auto jRoot = json::parse(m_stream);
                std::string type;
                unsigned version;

                jRoot.at("_type").get_to(type);
                jRoot.at("_version").get_to(version);

                if (type != "leaderboard" || version != 1u)
                {
                    std::cerr << std::format("Tried to load leaderboard \"{}\" but did not find expected type leaderboard of version 1\n", leaderboardDef.name);
                    return false;
                }

                const auto jLeaderboard = jRoot.get<JsonLeaderboardDef>();
                return CreateLeaderboardFromJson(jLeaderboard, leaderboardDef);
            }
            catch (const json::exception& e)
            {
                std::cerr << std::format("Failed to parse json of leaderboard: {}\n", e.what());
            }

            return false;
        }

    private:
        bool CreateColumnDefFromJson(const JsonColumnDef& jColumn, LbColumnDef& lbColumnDef, LeaderboardDef& leaderboardDef) const
        {
            lbColumnDef.name = m_memory.Dup(jColumn.name.c_str());

            lbColumnDef.id = jColumn.colId;
            lbColumnDef.propertyId = jColumn.propertyId.value_or(0);
            lbColumnDef.hidden = jColumn.hidden.value_or(false);

            if (jColumn.statName)
                lbColumnDef.statName = m_memory.Dup(jColumn.statName->c_str());
            else
                lbColumnDef.statName = nullptr;

            lbColumnDef.type = jColumn.type;

            lbColumnDef.precision = jColumn.precision.value_or(0);
            lbColumnDef.agg = jColumn.aggregationFunction;

            return true;
        }

        bool CreateLeaderboardFromJson(const JsonLeaderboardDef& jLeaderboardDef, LeaderboardDef& leaderboardDef) const
        {
            leaderboardDef.id = jLeaderboardDef.id;

            leaderboardDef.xpColId = jLeaderboardDef.xpColId.value_or(-1);
            leaderboardDef.prestigeColId = jLeaderboardDef.prestigeColId.value_or(-1);

            if (!jLeaderboardDef.columns.empty())
            {
                leaderboardDef.columnCount = static_cast<int>(jLeaderboardDef.columns.size());
                leaderboardDef.columns = m_memory.Alloc<LbColumnDef>(leaderboardDef.columnCount);

                for (auto i = 0; i < leaderboardDef.columnCount; i++)
                {
                    if (!CreateColumnDefFromJson(jLeaderboardDef.columns[i], leaderboardDef.columns[i], leaderboardDef))
                        return false;
                }
            }
            else
            {
                leaderboardDef.columnCount = 0;
                leaderboardDef.columns = nullptr;
            }

            return true;
        }

        std::istream& m_stream;
        MemoryManager& m_memory;
    };
} // namespace

namespace IW4
{
    bool LoadLeaderboardAsJson(std::istream& stream, LeaderboardDef& leaderboard, MemoryManager* memory)
    {
        const JsonLoader loader(stream, *memory);

        return loader.Load(leaderboard);
    }
} // namespace IW4
