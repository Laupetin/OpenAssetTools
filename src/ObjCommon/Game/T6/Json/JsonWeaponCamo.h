#pragma once

#include "Game/T6/T6.h"

#include "Json/JsonCommon.h"
#include "Json/JsonOptional.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace T6
{
    class JsonWeaponCamoSet
    {
    public:
        std::optional<std::string> solidCamoImage;
        std::optional<std::string> patternCamoImage;
        JsonVec2 patternOffset;
        float patternScale;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonWeaponCamoSet, solidCamoImage, patternCamoImage, patternOffset, patternScale);

    class JsonWeaponCamoMaterial
    {
    public:
        unsigned replaceFlags;
        std::vector<std::string> baseMaterials;
        std::vector<std::string> camoMaterials;
        std::array<float, 8> shaderConsts;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonWeaponCamoMaterial, replaceFlags, baseMaterials, camoMaterials, shaderConsts);

    class JsonWeaponCamoMaterialSet
    {
    public:
        std::vector<JsonWeaponCamoMaterial> materials;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonWeaponCamoMaterialSet, materials);

    class JsonWeaponCamo
    {
    public:
        std::optional<std::string> solidBaseImage;
        std::optional<std::string> patternBaseImage;
        std::vector<JsonWeaponCamoSet> camoSets;
        std::vector<JsonWeaponCamoMaterialSet> camoMaterials;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonWeaponCamo, solidBaseImage, patternBaseImage, camoSets, camoMaterials);
} // namespace T6
