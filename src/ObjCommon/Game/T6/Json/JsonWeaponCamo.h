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

    class JsonWeaponCamoMaterialOverride
    {
    public:
        std::string baseMaterial;
        std::string camoMaterial;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonWeaponCamoMaterialOverride, baseMaterial, camoMaterial);

    class JsonWeaponCamoMaterial
    {
    public:
        bool useColorMap;
        bool useNormalMap;
        bool useSpecularMap;
        std::vector<JsonWeaponCamoMaterialOverride> materialOverrides;
        std::array<float, 8> shaderConsts;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JsonWeaponCamoMaterial, useColorMap, useNormalMap, useSpecularMap, materialOverrides, shaderConsts);

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
