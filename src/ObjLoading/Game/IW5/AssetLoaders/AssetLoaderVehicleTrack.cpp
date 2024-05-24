#include "AssetLoaderVehicleTrack.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderVehicleTrack::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* vehicleTrack = memory->Create<VehicleTrack>();
    memset(vehicleTrack, 0, sizeof(VehicleTrack));
    vehicleTrack->name = memory->Dup(assetName.c_str());
    return vehicleTrack;
}
