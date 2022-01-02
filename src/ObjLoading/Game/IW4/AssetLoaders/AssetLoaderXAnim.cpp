#include "AssetLoaderXAnim.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderXAnim::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* anim = memory->Create<XAnimParts>();
    memset(anim, 0, sizeof(XAnimParts));
    anim->name = memory->Dup(assetName.c_str());
    return anim;
}
