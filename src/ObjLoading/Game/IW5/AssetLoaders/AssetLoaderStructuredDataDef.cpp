#include "AssetLoaderStructuredDataDef.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderStructuredDataDef::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* structuredDataDefSet = memory->Create<StructuredDataDefSet>();
    memset(structuredDataDefSet, 0, sizeof(StructuredDataDefSet));
    structuredDataDefSet->name = memory->Dup(assetName.c_str());
    return structuredDataDefSet;
}
