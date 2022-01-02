#include "AssetLoaderGfxLightDef.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderGfxLightDef::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* lightDef = memory->Create<GfxLightDef>();
    memset(lightDef, 0, sizeof(GfxLightDef));
    lightDef->name = memory->Dup(assetName.c_str());
    return lightDef;
}
