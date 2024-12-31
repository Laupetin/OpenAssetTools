#include "JsonLeaderboardDefLoader.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Leaderboard/JsonLeaderboardDef.h"

#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;

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

            try
            {
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
        static bool CreateTrackTypeFlagsFromJson(const JsonLeaderboardDef& jLeaderboardDef, int& trackTypeFlags)
        {
            for (const auto trackType : jLeaderboardDef.trackTypes)
                trackTypeFlags |= 1 << trackType;

            return true;
        }

        bool CreateColumnDefFromJson(const JsonColumnDef& jColumn, LbColumnDef& lbColumnDef, LeaderboardDef& leaderboardDef) const
        {
            lbColumnDef.name = m_memory.Dup(jColumn.name.c_str());
            lbColumnDef.colId = jColumn.colId;

            lbColumnDef.dwColIndex = jColumn.dwColIndex.value_or(-1);
            lbColumnDef.hidden = jColumn.hidden.value_or(false);

            if (jColumn.statName)
                lbColumnDef.statName = m_memory.Dup(jColumn.statName->c_str());
            else
                lbColumnDef.statName = nullptr;

            lbColumnDef.type = jColumn.type;

            lbColumnDef.precision = jColumn.precision.value_or(0);

            lbColumnDef.agg = jColumn.aggregationFunction;

            if (jColumn.localization)
                lbColumnDef.localization = m_memory.Dup(jColumn.localization->c_str());
            else
                lbColumnDef.localization = nullptr;

            lbColumnDef.uiCalColX = jColumn.uiCalColX.value_or(0);
            lbColumnDef.uiCalColY = jColumn.uiCalColY.value_or(0);

            return true;
        }

        bool CreateLeaderboardFromJson(const JsonLeaderboardDef& jLeaderboardDef, LeaderboardDef& leaderboardDef) const
        {
            leaderboardDef.id = jLeaderboardDef.id;
            leaderboardDef.dwColumnCount = jLeaderboardDef.dwColumnCount;

            leaderboardDef.xpColId = jLeaderboardDef.xpColId.value_or(-1);
            leaderboardDef.prestigeColId = jLeaderboardDef.prestigeColId.value_or(-1);

            if (!jLeaderboardDef.columns.empty())
            {
                leaderboardDef.columnCount = static_cast<int>(jLeaderboardDef.columns.size());
                leaderboardDef.columns = m_memory.Alloc<LbColumnDef>(leaderboardDef.columnCount);

                for (auto i = 0; i < leaderboardDef.columnCount; ++i)
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

            leaderboardDef.updateType = jLeaderboardDef.updateType;

            if (!CreateTrackTypeFlagsFromJson(jLeaderboardDef, leaderboardDef.trackTypes))
                return false;

            return true;
        }

        std::istream& m_stream;
        MemoryManager& m_memory;
    };
} // namespace

namespace T6
{
    bool LoadLeaderboardAsJson(std::istream& stream, LeaderboardDef& leaderboard, MemoryManager& memory)
    {
        const JsonLoader loader(stream, memory);
        return loader.Load(leaderboard);
    }
} // namespace T6
