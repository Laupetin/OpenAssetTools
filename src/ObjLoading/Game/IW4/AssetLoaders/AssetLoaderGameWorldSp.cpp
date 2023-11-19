#include "AssetLoaderGameWorldSp.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderGameWorldSp::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* gameWorld = memory->Create<GameWorldSp>();
    memset(gameWorld, 0, sizeof(GameWorldSp));
    gameWorld->name = memory->Dup(assetName.c_str());
    return gameWorld;
}
