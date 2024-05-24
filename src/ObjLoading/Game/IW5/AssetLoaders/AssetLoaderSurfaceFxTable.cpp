#include "AssetLoaderSurfaceFxTable.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderSurfaceFxTable::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* surfaceFxTable = memory->Create<SurfaceFxTable>();
    memset(surfaceFxTable, 0, sizeof(SurfaceFxTable));
    surfaceFxTable->name = memory->Dup(assetName.c_str());
    return surfaceFxTable;
}
