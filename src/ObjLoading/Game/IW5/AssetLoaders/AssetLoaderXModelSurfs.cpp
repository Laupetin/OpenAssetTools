#include "AssetLoaderXModelSurfs.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderXModelSurfs::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* modelSurfs = memory->Create<XModelSurfs>();
    memset(modelSurfs, 0, sizeof(XModelSurfs));
    modelSurfs->name = memory->Dup(assetName.c_str());
    return modelSurfs;
}
