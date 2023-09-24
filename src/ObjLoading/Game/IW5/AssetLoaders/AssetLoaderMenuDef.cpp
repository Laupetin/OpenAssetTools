#include "AssetLoaderMenuDef.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW5/IW5.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW5;

void* AssetLoaderMenuDef::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* menu = memory->Create<menuDef_t>();
    memset(menu, 0, sizeof(menuDef_t));
    menu->window.name = memory->Dup(assetName.c_str());
    return menu;
}
