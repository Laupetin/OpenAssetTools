#pragma once

#include "XAssetInfo.h"
#include "Zone/ZoneTypes.h"

class IZoneAssetPools
{
public:
    virtual ~IZoneAssetPools() = default;

    virtual void* AddAsset(asset_type_t type, std::string name, void* asset, std::vector<std::string>& scriptStrings, std::vector<XAssetDependency>& dependencies) = 0;
    virtual void InitPoolStatic(asset_type_t type, size_t capacity) = 0;
    virtual void InitPoolDynamic(asset_type_t type) = 0;
};