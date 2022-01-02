#include "AssetLoaderStructuredDataDefSet.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderStructuredDataDefSet::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* structuredDataDefSet = memory->Create<StructuredDataDefSet>();
    memset(structuredDataDefSet, 0, sizeof(StructuredDataDefSet));
    structuredDataDefSet->name = memory->Dup(assetName.c_str());
    return structuredDataDefSet;
}
