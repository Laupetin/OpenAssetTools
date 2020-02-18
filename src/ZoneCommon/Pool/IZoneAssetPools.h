#pragma once

#include "XAssetInfo.h"
#include "Zone/ZoneTypes.h"
#include "Zone/ZoneContent.h"
#include <vector>
#include <string>

class IZoneAssetPools
{
public:
    virtual ~IZoneAssetPools() = default;

    virtual XAssetInfoGeneric* AddAsset(asset_type_t type, std::string name, void* asset, std::vector<std::string>& scriptStrings, std::vector<XAssetInfoGeneric*>& dependencies) = 0;
    virtual XAssetInfoGeneric* GetAsset(asset_type_t type, std::string name) = 0;
    virtual void InitPoolStatic(asset_type_t type, size_t capacity) = 0;
    virtual void InitPoolDynamic(asset_type_t type) = 0;
    virtual ZoneContent GetContent() const = 0;
};
