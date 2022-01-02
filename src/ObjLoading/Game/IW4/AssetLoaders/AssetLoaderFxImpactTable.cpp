#include "AssetLoaderFxImpactTable.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderFxImpactTable::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* fxImpactTable = memory->Create<FxImpactTable>();
    memset(fxImpactTable, 0, sizeof(FxImpactTable));
    fxImpactTable->name = memory->Dup(assetName.c_str());
    return fxImpactTable;
}
