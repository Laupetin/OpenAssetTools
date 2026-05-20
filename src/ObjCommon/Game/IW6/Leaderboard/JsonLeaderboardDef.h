#pragma once

#include "Game/IW6/IW6.h"

#include "Json/JsonCommon.h"
#include "Json/JsonExtension.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace IW6
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

    NLOHMANN_JSON_SERIALIZE_ENUM(LbTrackTypes,
                                 {
                                     {LBTRK_TYPE_ALLTIME,           "ALLTIME"         },
                                     {LBTRK_TYPE_WEEKLY,            "WEEKLY"          },
                                     {LBTRK_TYPE_MONTHLY,           "MONTHLY"         },
                                     {LBTRK_TYPE_PRESTIGE_ALLTIME,  "PRESTIGE_ALLTIME"},
                                     {LBTRK_TYPE_PRESTIGE_WEEKLY,   "PRESTIGE_WEEKLY" },
                                     {LBTRK_TYPE_PRESTIGE_MONTHLY,  "PRESTIGE_MONTHLY"},
                                     {LBTRK_TYPE_DAILY,             "DAILY"           },
                                     {LBTRK_TYPE_PRESTIGE_DAILY,    "PRESTIGE_DAILY"  },
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
        std::vector<LbTrackTypes> trackTypes;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonLeaderboardDef, id, xpColId, prestigeColId, columns, updateType, trackTypes);
} // namespace IW6
