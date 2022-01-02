#include "AssetLoaderXModelSurfs.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderXModelSurfs::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* modelSurfs = memory->Create<XModelSurfs>();
    memset(modelSurfs, 0, sizeof(XModelSurfs));
    modelSurfs->name = memory->Dup(assetName.c_str());
    return modelSurfs;
}
