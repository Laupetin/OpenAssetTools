#include "AssetLoaderXModel.h"

#include "Game/T5/T5.h"
#include "Game/T5/XModel/XModelLoaderT5.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T5;

void* AssetLoaderXModel::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetXModel::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}

bool AssetLoaderXModel::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderXModel::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(std::format("xmodel/{}.json", assetName));
    if (!file.IsOpen())
        return false;

    auto* xmodel = memory->Alloc<XModel>();
    xmodel->name = memory->Dup(assetName.c_str());

    std::vector<XAssetInfoGeneric*> dependencies;
    if (LoadXModel(*file.m_stream, *xmodel, memory, manager, dependencies))
    {
        manager->AddAsset<AssetXModel>(assetName, xmodel, std::move(dependencies));
    }
    else
    {
        std::cerr << std::format("Failed to load xmodel \"{}\"\n", assetName);
        return false;
    }

    return true;
}
