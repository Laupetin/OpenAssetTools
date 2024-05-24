#include "AssetLoaderXModel.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderXModel::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* model = memory->Create<XModel>();
    memset(model, 0, sizeof(XModel));
    model->name = memory->Dup(assetName.c_str());
    return model;
}
