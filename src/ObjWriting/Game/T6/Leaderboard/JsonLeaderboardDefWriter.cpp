#include "JsonLeaderboardDefWriter.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Leaderboard/JsonLeaderboardDef.h"

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

        void Dump(const LeaderboardDef* leaderboardDef) const
        {
            JsonLeaderboardDef jsonLeaderboardDef;
            CreateJsonLeaderboardDef(jsonLeaderboardDef, *leaderboardDef);

            json jRoot = jsonLeaderboardDef;

            jRoot["_type"] = "leaderboard";
            jRoot["_version"] = 1;
            jRoot["_game"] = "t6";

            m_stream << std::setw(4) << jRoot << "\n";
        }

    private:
        static void CreateJsonColumnDef(JsonColumnDef& jColumnDef, const LbColumnDef& lbColumnDef)
        {
            jColumnDef.name = lbColumnDef.name;
            jColumnDef.colId = lbColumnDef.colId;

            if (lbColumnDef.dwColIndex >= 0)
                jColumnDef.dwColIndex = lbColumnDef.dwColIndex;

            if (lbColumnDef.hidden)
                jColumnDef.hidden = lbColumnDef.hidden;

            if (lbColumnDef.statName && lbColumnDef.statName[0])
                jColumnDef.statName = lbColumnDef.statName;

            jColumnDef.type = lbColumnDef.type;

            if (lbColumnDef.precision != 0)
                jColumnDef.precision = lbColumnDef.precision;

            jColumnDef.aggregationFunction = lbColumnDef.agg;

            if (lbColumnDef.localization && lbColumnDef.localization[0])
                jColumnDef.localization = lbColumnDef.localization;

            if (lbColumnDef.uiCalColX != 0 || lbColumnDef.uiCalColY != 0)
            {
                jColumnDef.uiCalColX = lbColumnDef.uiCalColX;
                jColumnDef.uiCalColY = lbColumnDef.uiCalColY;
            }
        }

        static void CreateJsonLeaderboardDef(JsonLeaderboardDef& jLeaderboardDef, const LeaderboardDef& leaderboardDef)
        {
            jLeaderboardDef.id = leaderboardDef.id;
            jLeaderboardDef.dwColumnCount = leaderboardDef.dwColumnCount;

            if (leaderboardDef.xpColId >= 0)
                jLeaderboardDef.xpColId = leaderboardDef.xpColId;
            if (leaderboardDef.prestigeColId >= 0)
                jLeaderboardDef.prestigeColId = leaderboardDef.prestigeColId;

            jLeaderboardDef.columns.resize(leaderboardDef.columnCount);
            for (auto i = 0; i < leaderboardDef.columnCount; ++i)
                CreateJsonColumnDef(jLeaderboardDef.columns[i], leaderboardDef.columns[i]);

            jLeaderboardDef.updateType = leaderboardDef.updateType;

            for (auto i = 0; i < TRK_COUNT; ++i)
            {
                const auto trackTypeMask = 1 << i;

                if (leaderboardDef.trackTypes & trackTypeMask)
                    jLeaderboardDef.trackTypes.emplace_back(static_cast<LbTrackType>(i));
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
