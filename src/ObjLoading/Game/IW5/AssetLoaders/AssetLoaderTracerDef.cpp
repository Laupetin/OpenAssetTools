#include "AssetLoaderTracerDef.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderTracerDef::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* tracerDef = memory->Create<TracerDef>();
    memset(tracerDef, 0, sizeof(TracerDef));
    tracerDef->name = memory->Dup(assetName.c_str());
    return tracerDef;
}
