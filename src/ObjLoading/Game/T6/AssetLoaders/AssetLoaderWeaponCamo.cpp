#include "AssetLoaderWeaponCamo.h"

#include "Game/T6/T6.h"
#include "Game/T6/WeaponCamo/JsonWeaponCamoLoader.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

void* AssetLoaderWeaponCamo::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* weaponCamo = memory->Create<WeaponCamo>();
    memset(weaponCamo, 0, sizeof(WeaponCamo));
    weaponCamo->name = memory->Dup(assetName.c_str());

    return weaponCamo;
}

bool AssetLoaderWeaponCamo::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderWeaponCamo::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto fileName = std::format("camo/{}.json", assetName);
    const auto file = searchPath->Open(fileName);
    if (!file.IsOpen())
        return false;

    auto* weaponCamo = memory->Alloc<WeaponCamo>();
    weaponCamo->name = memory->Dup(assetName.c_str());

    std::vector<XAssetInfoGeneric*> dependencies;
    if (LoadWeaponCamoAsJson(*file.m_stream, *weaponCamo, memory, manager, dependencies))
        manager->AddAsset<AssetWeaponCamo>(assetName, weaponCamo, std::move(dependencies));
    else
        std::cerr << "Failed to load weapon camo \"" << assetName << "\"\n";

    return true;
}
