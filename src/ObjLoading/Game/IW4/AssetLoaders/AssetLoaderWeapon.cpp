#include "AssetLoaderWeapon.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderWeapon::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* weapon = memory->Create<WeaponCompleteDef>();
    memset(weapon, 0, sizeof(WeaponCompleteDef));
    weapon->szInternalName = memory->Dup(assetName.c_str());
    return weapon;
}
