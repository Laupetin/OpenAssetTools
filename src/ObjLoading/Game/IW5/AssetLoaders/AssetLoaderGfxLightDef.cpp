#include "AssetLoaderGfxLightDef.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderGfxLightDef::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* lightDef = memory->Create<GfxLightDef>();
    memset(lightDef, 0, sizeof(GfxLightDef));
    lightDef->name = memory->Dup(assetName.c_str());
    return lightDef;
}
