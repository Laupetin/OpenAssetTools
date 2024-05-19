#pragma once

#include "Game/IW5/IW5.h"

#include "Json/JsonCommon.h"
#include "Json/JsonExtension.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace IW5
{
    NLOHMANN_JSON_SERIALIZE_ENUM(LbColType,
                                 {
                                     {LBCOL_TYPE_NUMBER,    "number"   },
                                     {LBCOL_TYPE_TIME,      "time"     },
                                     {LBCOL_TYPE_LEVELXP,   "levelxp"  },
                                     {LBCOL_TYPE_PRESTIGE,  "prestige" },
                                     {LBCOL_TYPE_BIGNUMBER, "bignumber"},
                                     {LBCOL_TYPE_PERCENT,   "percent"  },
                                     {LBCOL_TYPE_TIME_FULL, "time_full"},
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(LbAggType,
                                 {
                                     {LBAGG_TYPE_MIN,  "min" },
                                     {LBAGG_TYPE_MAX,  "max" },
                                     {LBAGG_TYPE_SUM,  "sum" },
                                     {LBAGG_TYPE_LAST, "last"},
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(LbUpdateType,
                                 {
                                     {LBUPDATE_TYPE_NORMAL,  "normal" },
                                     {LBUPDATE_TYPE_RANK,    "rank"   },
                                     {LBUPDATE_TYPE_COMBINE, "combine"},
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(LbTrackType,
                                 {
                                     {TRK_ALLTIME,          "ALLTIME"         },
                                     {TRK_WEEKLY,           "WEEKLY"          },
                                     {TRK_MONTHLY,          "MONTHLY"         },
                                     {TRK_PRESTIGE_ALLTIME, "PRESTIGE_ALLTIME"},
                                     {TRK_PRESTIGE_WEEKLY,  "PRESTIGE_WEEKLY" },
                                     {TRK_PRESTIGE_MONTHLY, "PRESTIGE_MONTHLY"},
                                     {TRK_DAILY,            "DAILY"           },
                                     {TRK_PRESTIGE_DAILY,   "PRESTIGE_DAILY"  },
    });

    class JsonColumnDef
    {
    public:
        std::string name;
        int colId;
        std::optional<int> propertyId;
        std::optional<bool> hidden;
        std::optional<std::string> statName;
        LbColType type;
        std::optional<int> precision;
        LbAggType aggregationFunction;
        std::optional<int> uiCalColX;
        std::optional<int> uiCalColY;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonColumnDef, name, colId, propertyId, hidden, statName, type, precision, aggregationFunction, uiCalColX, uiCalColY);

    class JsonLeaderboardDef
    {
    public:
        int id;
        std::optional<int> xpColId;
        std::optional<int> prestigeColId;
        std::vector<JsonColumnDef> columns;
        LbUpdateType updateType;
        std::vector<LbTrackType> trackTypes;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonLeaderboardDef, id, xpColId, prestigeColId, columns, updateType, trackTypes);
} // namespace IW5
