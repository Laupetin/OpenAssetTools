#include "AssetLoaderVehicle.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderVehicle::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vehicle = memory->Create<VehicleDef>();
    memset(vehicle, 0, sizeof(VehicleDef));
    vehicle->name = memory->Dup(assetName.c_str());
    return vehicle;
}
