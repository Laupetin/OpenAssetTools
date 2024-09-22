#pragma once

#include "Json/JsonCommon.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace T5
{
    class JsonXModelLod
    {
    public:
        std::string file;
        float distance;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonXModelLod, file, distance);

    class JsonXModel
    {
    public:
        std::vector<JsonXModelLod> lods;
        std::optional<int> collLod;
        std::optional<std::string> physPreset;
        std::optional<std::string> physConstraints;
        unsigned flags;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonXModel, lods, collLod, physPreset, physConstraints, flags);
} // namespace T5
