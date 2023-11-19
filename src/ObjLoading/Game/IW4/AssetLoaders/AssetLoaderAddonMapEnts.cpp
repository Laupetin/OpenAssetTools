#include "AssetLoaderAddonMapEnts.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderAddonMapEnts::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* addonMapEnts = memory->Create<AddonMapEnts>();
    memset(addonMapEnts, 0, sizeof(AddonMapEnts));
    addonMapEnts->name = memory->Dup(assetName.c_str());
    return addonMapEnts;
}
