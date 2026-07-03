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
    class JsonGlyph
    {
    public:
        std::string letter;
        int x0;
        int y0;
        unsigned dx;
        unsigned pixelWidth;
        unsigned pixelHeight;
        float s0;
        float t0;
        float s1;
        float t1;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonGlyph, letter, x0, y0, dx, pixelWidth, pixelHeight, s0, t0, s1, t1);

    class JsonKerningPair
    {
    public:
        unsigned wFirst;
        unsigned wSecond;
        int iKernAmount;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonKerningPair, wFirst, wSecond, iKernAmount);

    class JsonFont
    {
    public:
        int pixelHeight;
        bool isScalingAllowed;
        std::optional<std::string> material;
        std::optional<std::string> glowMaterial;
        std::vector<JsonGlyph> glyphs;
        std::vector<JsonKerningPair> kerningPairs;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonFont, pixelHeight, isScalingAllowed, material, glowMaterial, glyphs, kerningPairs);
} // namespace T6
