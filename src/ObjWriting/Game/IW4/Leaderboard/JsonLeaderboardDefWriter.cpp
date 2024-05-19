#include "JsonLeaderboardDefWriter.h"

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/Leaderboard/JsonLeaderboardDef.h"

#include <iomanip>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace IW4;

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

            m_stream << std::setw(4) << jRoot << "\n";
        }

    private:
        static void CreateJsonColumnDef(JsonColumnDef& jColumnDef, const LbColumnDef& lbColumnDef)
        {
            jColumnDef.name = lbColumnDef.name;
            jColumnDef.id = lbColumnDef.id;

            if (lbColumnDef.propertyId != 0)
                jColumnDef.propertyId = lbColumnDef.propertyId;

            if (lbColumnDef.hidden)
                jColumnDef.hidden = lbColumnDef.hidden;

            if (lbColumnDef.statName && lbColumnDef.statName[0])
                jColumnDef.statName = lbColumnDef.statName;

            jColumnDef.type = lbColumnDef.type;

            if (lbColumnDef.precision != 0)
                jColumnDef.precision = lbColumnDef.precision;

            jColumnDef.aggregationFunction = lbColumnDef.agg;
        }

        static void CreateJsonLeaderboardDef(JsonLeaderboardDef& jLeaderboardDef, const LeaderboardDef& leaderboardDef)
        {
            jLeaderboardDef.id = leaderboardDef.id;

            if (leaderboardDef.xpColId >= 0)
                jLeaderboardDef.xpColId = leaderboardDef.xpColId;
            if (leaderboardDef.prestigeColId >= 0)
                jLeaderboardDef.prestigeColId = leaderboardDef.prestigeColId;

            jLeaderboardDef.columns.resize(leaderboardDef.columnCount);
            for (auto i = 0; i < leaderboardDef.columnCount; ++i)
                CreateJsonColumnDef(jLeaderboardDef.columns[i], leaderboardDef.columns[i]);
        }

        std::ostream& m_stream;
    };
} // namespace

namespace IW4
{
    void DumpLeaderboardDefAsJson(std::ostream& stream, const LeaderboardDef* leaderboardDef)
    {
        JsonDumper dumper(stream);
        dumper.Dump(leaderboardDef);
    }
} // namespace IW4
