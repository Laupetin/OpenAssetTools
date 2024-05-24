#include "AssetLoaderTechniqueSet.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderTechniqueSet::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* techniqueSet = memory->Create<MaterialTechniqueSet>();
    memset(techniqueSet, 0, sizeof(MaterialTechniqueSet));
    techniqueSet->name = memory->Dup(assetName.c_str());
    return techniqueSet;
}
