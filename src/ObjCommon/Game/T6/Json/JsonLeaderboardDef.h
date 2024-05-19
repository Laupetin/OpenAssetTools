#pragma once

#include "Game/T6/T6.h"

#include "Json/JsonCommon.h"
#include "Json/JsonExtension.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace T6
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
                                     {LBAGG_TYPE_MIN,     "min"    },
                                     {LBAGG_TYPE_MAX,     "max"    },
                                     {LBAGG_TYPE_ADD,     "add"    },
                                     {LBAGG_TYPE_REPLACE, "replace"},
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
        int dwColIndex;
        bool hidden;
        std::string statName;
        LbColType type;
        int precision;
        LbAggType agg;
        std::string localization;
        int uiCalColX;
        int uiCalColY;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonColumnDef, name, colId, dwColIndex, hidden, statName, type, precision, agg, localization, uiCalColX, uiCalColY);

    class JsonLeaderboardDef
    {
    public:
        unsigned int id;
        int dwColumnCount;
        std::optional<int> xpColId;
        std::optional<int> prestigeColId;
        std::vector<JsonColumnDef> columns;
        LbUpdateType updateType;
        std::vector<LbTrackType> trackTypes;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonLeaderboardDef, id, dwColumnCount, xpColId, prestigeColId, columns, updateType, trackTypes);
} // namespace T6
