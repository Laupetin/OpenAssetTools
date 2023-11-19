#include "AssetLoaderXModel.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderXModel::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* model = memory->Create<XModel>();
    memset(model, 0, sizeof(XModel));
    model->name = memory->Dup(assetName.c_str());
    return model;
}
