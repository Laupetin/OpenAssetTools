#include "AssetLoaderVehicleDef.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderVehicleDef::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vehicleDef = memory->Create<VehicleDef>();
    memset(vehicleDef, 0, sizeof(VehicleDef));
    vehicleDef->name = memory->Dup(assetName.c_str());
    return vehicleDef;
}
