#include "AssetLoaderMaterial.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW5/IW5.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW5;

void* AssetLoaderMaterial::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* material = memory->Create<Material>();
    memset(material, 0, sizeof(Material));
    material->info.name = memory->Dup(assetName.c_str());

    return material;
}
