#include "JsonLeaderboardDefWriter.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonLeaderboardDef.h"

#include <iomanip>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;

namespace
{
    class JsonDumper
    {
    public:
        explicit JsonDumper(std::ostream& stream)
            : m_stream(stream)
        {
        }

        void Dump(const LeaderboardDef* leaderboardDef)
        {
            JsonLeaderboardDef jsonLeaderboardDef;
            CreateJsonLeaderboardDef(jsonLeaderboardDef, *leaderboardDef);

            json jRoot = jsonLeaderboardDef;

            jRoot["_type"] = "leaderboard";
            jRoot["_version"] = 1;

            m_stream << std::setw(4) << jRoot << "\n";
        }

    private:
        static void CreateJsonColumnDef(JsonColumnDef& jColumnDef, const LbColumnDef& lbColumnDef)
        {
            jColumnDef.name = lbColumnDef.name;
            jColumnDef.colId = lbColumnDef.colId;
            jColumnDef.dwColIndex = lbColumnDef.dwColIndex;
            jColumnDef.hidden = lbColumnDef.hidden;
            jColumnDef.statName = lbColumnDef.statName;
            jColumnDef.type = lbColumnDef.type;
            jColumnDef.precision = lbColumnDef.precision;
            jColumnDef.agg = lbColumnDef.agg;
            jColumnDef.localization = lbColumnDef.localization;
            jColumnDef.uiCalColX = lbColumnDef.uiCalColX;
            jColumnDef.uiCalColY = lbColumnDef.uiCalColY;
        }

        static void CreateJsonLeaderboardDef(JsonLeaderboardDef& jLeaderboardDef, const LeaderboardDef& leaderboardDef)
        {
            jLeaderboardDef.id = leaderboardDef.id;
            jLeaderboardDef.dwColumnCount = leaderboardDef.dwColumnCount;
            jLeaderboardDef.xpColId = (leaderboardDef.xpColId < 0) ? std::nullopt : std::make_optional(leaderboardDef.xpColId);
            jLeaderboardDef.prestigeColId = (leaderboardDef.prestigeColId < 0) ? std::nullopt : std::make_optional(leaderboardDef.prestigeColId);

            jLeaderboardDef.columns.resize(leaderboardDef.columnCount);
            for (auto i = 0; i < leaderboardDef.columnCount; ++i)
                CreateJsonColumnDef(jLeaderboardDef.columns[i], leaderboardDef.columns[i]);

            jLeaderboardDef.updateType = leaderboardDef.updateType;

            for (auto i = 0; i < LbTrackType::TRK_COUNT; ++i)
            {
                if ((leaderboardDef.trackTypes & (1 << i)) != 0)
                {
                    jLeaderboardDef.trackTypes.emplace_back(static_cast<LbTrackType>(i));
                }
            }
        }

        std::ostream& m_stream;
    };
} // namespace

namespace T6
{
    void DumpLeaderboardDefAsJson(std::ostream& stream, const LeaderboardDef* leaderboardDef)
    {
        JsonDumper dumper(stream);
        dumper.Dump(leaderboardDef);
    }
} // namespace T6
