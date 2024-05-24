#include "AssetLoaderLoadedSound.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderLoadedSound::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* loadedSound = memory->Create<LoadedSound>();
    memset(loadedSound, 0, sizeof(LoadedSound));
    loadedSound->name = memory->Dup(assetName.c_str());
    return loadedSound;
}
