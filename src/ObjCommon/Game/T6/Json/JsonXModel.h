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
    class JsonXModelLod
    {
    public:
        std::string file;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonXModelLod, file);

    class JsonXModel
    {
    public:
        std::vector<JsonXModelLod> lods;
        std::optional<int> collLod;
        std::optional<std::string> physPreset;
        std::optional<std::string> physConstraints;
        unsigned flags;
        JsonVec3 lightingOriginOffset;
        float lightingOriginRange;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonXModel, lods, collLod, physPreset, physConstraints, flags, lightingOriginOffset, lightingOriginRange);
} // namespace T6
