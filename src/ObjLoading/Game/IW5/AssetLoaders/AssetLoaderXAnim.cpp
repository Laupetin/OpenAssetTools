#include "AssetLoaderXAnim.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderXAnim::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* anim = memory->Create<XAnimParts>();
    memset(anim, 0, sizeof(XAnimParts));
    anim->name = memory->Dup(assetName.c_str());
    return anim;
}
