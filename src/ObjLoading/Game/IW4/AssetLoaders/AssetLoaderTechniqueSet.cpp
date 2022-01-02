#include "AssetLoaderTechniqueSet.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderTechniqueSet::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* techset = memory->Create<MaterialTechniqueSet>();
    memset(techset, 0, sizeof(MaterialTechniqueSet));
    techset->name = memory->Dup(assetName.c_str());
    return techset;
}
