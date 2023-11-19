#include "AssetLoaderTracer.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderTracer::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* tracer = memory->Create<TracerDef>();
    memset(tracer, 0, sizeof(TracerDef));
    tracer->name = memory->Dup(assetName.c_str());
    return tracer;
}
