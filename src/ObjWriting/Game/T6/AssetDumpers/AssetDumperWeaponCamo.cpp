#include "AssetDumperWeaponCamo.h"

#include "Game/T6/WeaponCamo/JsonWeaponCamoWriter.h"

#include <format>

using namespace T6;

bool AssetDumperWeaponCamo::ShouldDump(XAssetInfo<WeaponCamo>* asset)
{
    return true;
}

void AssetDumperWeaponCamo::DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponCamo>* asset)
{
    const auto fileName = std::format("camo/{}.json", asset->m_name);
    const auto assetFile = context.OpenAssetFile(fileName);

    if (!assetFile)
        return;

    DumpWeaponCamoAsJson(*assetFile, asset->Asset());
}
