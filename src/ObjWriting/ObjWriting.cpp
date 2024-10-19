#include "ObjWriting.h"

ObjWriting::Configuration_t ObjWriting::Configuration;

bool ObjWriting::ShouldHandleAssetType(const asset_type_t assetType)
{
    if (assetType < 0)
        return false;
    if (static_cast<size_t>(assetType) >= Configuration.AssetTypesToHandleBitfield.size())
        return true;

    return Configuration.AssetTypesToHandleBitfield[assetType];
}
