#pragma once

#include "Game/IW4/IW4.h"

#include "Json/JsonCommon.h"
#include "Json/JsonExtension.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace IW4
{
    NLOHMANN_JSON_SERIALIZE_ENUM(LbColType,
                                 {
                                     {LBCOL_TYPE_NUMBER,    "number"   },
                                     {LBCOL_TYPE_TIME,      "time"     },
                                     {LBCOL_TYPE_LEVELXP,   "levelxp"  },
                                     {LBCOL_TYPE_PRESTIGE,  "prestige" },
                                     {LBCOL_TYPE_BIGNUMBER, "bignumber"},
                                     {LBCOL_TYPE_PERCENT,   "percent"  },
    });

    NLOHMANN_JSON_SERIALIZE_ENUM(LbAggType,
                                 {
                                     {LBAGG_TYPE_MIN,  "min" },
                                     {LBAGG_TYPE_MAX,  "max" },
                                     {LBAGG_TYPE_SUM,  "sum" },
                                     {LBAGG_TYPE_LAST, "last"},
    });

    class JsonColumnDef
    {
    public:
        std::string name;
        int id;
        int propertyId;
        bool hidden;
        std::string statName;
        LbColType type;
        int precision;
        LbAggType aggregationFunction;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonColumnDef, name, id, propertyId, hidden, statName, type, precision, aggregationFunction);

    class JsonLeaderboardDef
    {
    public:
        int id;
        std::optional<int> xpColId;
        std::optional<int> prestigeColId;
        std::vector<JsonColumnDef> columns;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonLeaderboardDef, id, xpColId, prestigeColId, columns);
} // namespace IW4
