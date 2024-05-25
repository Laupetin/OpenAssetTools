#include "AssetLoaderXModel.h"

#include "Game/T6/T6.h"
#include "Game/T6/XModel/JsonXModelLoader.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

void* AssetLoaderXModel::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* xmodel = memory->Create<XModel>();
    memset(xmodel, 0, sizeof(XModel));
    xmodel->name = memory->Dup(assetName.c_str());

    return xmodel;
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
    if (LoadXModelAsJson(*file.m_stream, *xmodel, memory, manager, dependencies))
        manager->AddAsset<AssetXModel>(assetName, xmodel, std::move(dependencies));
    else
        std::cerr << std::format("Failed to load xmodel \"{}\"\n", assetName);

    return true;
}
