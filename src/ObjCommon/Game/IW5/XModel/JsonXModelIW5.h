#pragma once

#include "Json/JsonCommon.h"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace IW5
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
        std::optional<std::string> physCollmap;
        uint8_t flags;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonXModel, lods, collLod, physPreset, physCollmap, flags);
} // namespace IW5
