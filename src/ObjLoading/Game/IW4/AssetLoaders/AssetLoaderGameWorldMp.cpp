#include "AssetLoaderGameWorldMp.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderGameWorldMp::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* gameWorld = memory->Create<GameWorldMp>();
    memset(gameWorld, 0, sizeof(GameWorldMp));
    gameWorld->name = memory->Dup(assetName.c_str());
    return gameWorld;
}
