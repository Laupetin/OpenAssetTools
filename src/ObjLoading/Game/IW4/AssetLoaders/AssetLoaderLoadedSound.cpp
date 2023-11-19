#include "AssetLoaderLoadedSound.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderLoadedSound::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* loadedSound = memory->Create<LoadedSound>();
    memset(loadedSound, 0, sizeof(LoadedSound));
    loadedSound->name = memory->Dup(assetName.c_str());
    return loadedSound;
}
