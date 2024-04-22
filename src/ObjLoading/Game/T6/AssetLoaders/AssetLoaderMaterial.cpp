#include "AssetLoaderMaterial.h"

#include "Game/T6/Material/JsonMaterialLoader.h"
#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

void* AssetLoaderMaterial::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* material = memory->Create<Material>();
    memset(material, 0, sizeof(Material));
    material->info.name = memory->Dup(assetName.c_str());

    return material;
}

bool AssetLoaderMaterial::CanLoadFromRaw() const
{
    return true;
}

std::string AssetLoaderMaterial::GetFileNameForAsset(const std::string& assetName)
{
    std::string sanitizedFileName(assetName);
    if (sanitizedFileName[0] == '*')
    {
        std::ranges::replace(sanitizedFileName, '*', '_');
        const auto parenthesisPos = sanitizedFileName.find('(');
        if (parenthesisPos != std::string::npos)
            sanitizedFileName.erase(parenthesisPos);
        sanitizedFileName = "generated/" + sanitizedFileName;
    }

    return std::format("materials/{}.json", sanitizedFileName);
}

bool AssetLoaderMaterial::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(GetFileNameForAsset(assetName));
    if (!file.IsOpen())
        return false;

    auto* material = memory->Alloc<Material>();
    material->info.name = memory->Dup(assetName.c_str());

    std::vector<XAssetInfoGeneric*> dependencies;
    if (LoadMaterialAsJson(*file.m_stream, *material, memory, manager, dependencies))
        manager->AddAsset(ASSET_TYPE_MATERIAL, assetName, material, std::move(dependencies));
    else
        std::cerr << "Failed to load material \"" << assetName << "\"\n";

    return true;
}
