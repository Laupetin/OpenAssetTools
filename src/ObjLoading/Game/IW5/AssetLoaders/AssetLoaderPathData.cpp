#include "AssetLoaderPathData.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderPathData::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* pathData = memory->Create<PathData>();
    memset(pathData, 0, sizeof(PathData));
    pathData->name = memory->Dup(assetName.c_str());
    return pathData;
}
