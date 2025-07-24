#pragma once

#include "Json/JsonCommon.h"
#include "Json/JsonExtension.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace T6
{
    class JsonFontIconEntry
    {
    public:
        std::string name;
        std::string material;
        unsigned size;
        std::optional<JsonVec2> scale;
        std::vector<std::string> aliases;
        std::optional<std::vector<unsigned>> aliasHashes;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonFontIconEntry, name, material, size, scale, aliases, aliasHashes);

    class JsonFontIcon
    {
    public:
        std::vector<JsonFontIconEntry> entries;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonFontIcon, entries);
} // namespace T6
