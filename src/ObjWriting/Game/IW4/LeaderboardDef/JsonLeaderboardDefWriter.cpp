#include "JsonLeaderboardDefWriter.h"

#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/Json/JsonLeaderboardDef.h"

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
            jColumnDef.id = lbColumnDef.id;
            jColumnDef.propertyId = lbColumnDef.propertyId;
            jColumnDef.hidden = lbColumnDef.hidden;
            jColumnDef.statName = lbColumnDef.statName;
            jColumnDef.type = lbColumnDef.type;
            jColumnDef.precision = lbColumnDef.precision;
            jColumnDef.agg = lbColumnDef.agg;
        }

        static void CreateJsonLeaderboardDef(JsonLeaderboardDef& jLeaderboardDef, const LeaderboardDef& leaderboardDef)
        {
            jLeaderboardDef.id = leaderboardDef.id;
            jLeaderboardDef.xpColId = (leaderboardDef.xpColId < 0) ? std::nullopt : std::make_optional(leaderboardDef.xpColId);
            jLeaderboardDef.prestigeColId = (leaderboardDef.prestigeColId < 0) ? std::nullopt : std::make_optional(leaderboardDef.prestigeColId);

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
