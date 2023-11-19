#include "AssetLoaderSoundAliasList.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderSoundAliasList::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* sndAliasList = memory->Create<snd_alias_list_t>();
    memset(sndAliasList, 0, sizeof(snd_alias_list_t));
    sndAliasList->aliasName = memory->Dup(assetName.c_str());
    return sndAliasList;
}
