#include "CamoJsonDumperT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonWeaponCamo.h"
#include "Weapon/CamoCommon.h"

#include <iomanip>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;

namespace
{
    class JsonDumperImpl
    {
    public:
        explicit JsonDumperImpl(std::ostream& stream)
            : m_stream(stream)
        {
        }

        void Dump(const WeaponCamo* weaponCamo) const
        {
            JsonWeaponCamo jsonWeaponCamo;
            CreateJsonWeaponCamo(jsonWeaponCamo, *weaponCamo);
            json jRoot = jsonWeaponCamo;

            jRoot["_type"] = "weaponCamo";
            jRoot["_version"] = 1;
            jRoot["_game"] = "t6";

            m_stream << std::setw(4) << jRoot << "\n";
        }

    private:
        static const char* AssetName(const char* input)
        {
            if (input && input[0] == ',')
                return &input[1];

            return input;
        }

        static void CreateJsonWeaponCamoSet(JsonWeaponCamoSet& jWeaponCamoSet, const WeaponCamoSet& weaponCamoSet)
        {
            if (weaponCamoSet.solidCamoImage && weaponCamoSet.solidCamoImage->name)
                jWeaponCamoSet.solidCamoImage = AssetName(weaponCamoSet.solidCamoImage->name);

            if (weaponCamoSet.patternCamoImage && weaponCamoSet.patternCamoImage->name)
                jWeaponCamoSet.patternCamoImage = AssetName(weaponCamoSet.patternCamoImage->name);

            jWeaponCamoSet.patternOffset.x = weaponCamoSet.patternOffset.x;
            jWeaponCamoSet.patternOffset.y = weaponCamoSet.patternOffset.y;
            jWeaponCamoSet.patternScale = weaponCamoSet.patternScale;
        }

        static void CreateJsonWeaponCamoMaterial(JsonWeaponCamoMaterial& jWeaponCamoMaterial, const WeaponCamoMaterial& weaponCamoMaterial)
        {
            jWeaponCamoMaterial.useColorMap = weaponCamoMaterial.replaceFlags & WCM_REPLACE_COLOR;
            jWeaponCamoMaterial.useNormalMap = weaponCamoMaterial.replaceFlags & WCM_REPLACE_NORMAL;
            jWeaponCamoMaterial.useSpecularMap = weaponCamoMaterial.replaceFlags & WCM_REPLACE_SPECULAR;

            jWeaponCamoMaterial.materialOverrides.resize(weaponCamoMaterial.numBaseMaterials);
            for (auto i = 0u; i < weaponCamoMaterial.numBaseMaterials; i++)
            {
                if (weaponCamoMaterial.baseMaterials[i] && weaponCamoMaterial.baseMaterials[i]->info.name)
                    jWeaponCamoMaterial.materialOverrides[i].baseMaterial = AssetName(weaponCamoMaterial.baseMaterials[i]->info.name);
                if (weaponCamoMaterial.camoMaterials[i] && weaponCamoMaterial.camoMaterials[i]->info.name)
                    jWeaponCamoMaterial.materialOverrides[i].camoMaterial = AssetName(weaponCamoMaterial.camoMaterials[i]->info.name);
            }

            for (auto i = 0u; i < std::extent_v<decltype(WeaponCamoMaterial::shaderConsts)>; i++)
                jWeaponCamoMaterial.shaderConsts[i] = weaponCamoMaterial.shaderConsts[i];
        }

        static void CreateJsonWeaponCamoMaterialSet(JsonWeaponCamoMaterialSet& jWeaponCamoMaterialSet, const WeaponCamoMaterialSet& weaponCamoMaterialSet)
        {
            jWeaponCamoMaterialSet.materials.resize(weaponCamoMaterialSet.numMaterials);
            for (auto i = 0u; i < weaponCamoMaterialSet.numMaterials; i++)
                CreateJsonWeaponCamoMaterial(jWeaponCamoMaterialSet.materials[i], weaponCamoMaterialSet.materials[i]);
        }

        static void CreateJsonWeaponCamo(JsonWeaponCamo& jWeaponCamo, const WeaponCamo& weaponCamo)
        {
            if (weaponCamo.solidBaseImage && weaponCamo.solidBaseImage->name)
                jWeaponCamo.solidBaseImage = AssetName(weaponCamo.solidBaseImage->name);

            if (weaponCamo.patternBaseImage && weaponCamo.patternBaseImage->name)
                jWeaponCamo.patternBaseImage = AssetName(weaponCamo.patternBaseImage->name);

            jWeaponCamo.camoSets.resize(weaponCamo.numCamoSets);
            for (auto i = 0u; i < weaponCamo.numCamoSets; i++)
                CreateJsonWeaponCamoSet(jWeaponCamo.camoSets[i], weaponCamo.camoSets[i]);

            jWeaponCamo.camoMaterials.resize(weaponCamo.numCamoMaterials);
            for (auto i = 0u; i < weaponCamo.numCamoMaterials; i++)
                CreateJsonWeaponCamoMaterialSet(jWeaponCamo.camoMaterials[i], weaponCamo.camoMaterials[i]);
        }

        std::ostream& m_stream;
    };
} // namespace

namespace camo
{
    void JsonDumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetWeaponCamo::Type>& asset)
    {
        const auto fileName = GetJsonFileNameForAssetName(asset.m_name);
        const auto assetFile = context.OpenAssetFile(fileName);

        if (!assetFile)
            return;

        const JsonDumperImpl dumper(*assetFile);
        dumper.Dump(asset.Asset());
    }
} // namespace camo
