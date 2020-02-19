#pragma once

#include "XAssetInfo.h"
#include "Zone/ZoneTypes.h"
#include <vector>
#include <string>

class IZoneAssetPools
{
public:
    using iterator = std::vector<XAssetInfoGeneric*>::const_iterator;

    virtual ~IZoneAssetPools() = default;

    virtual XAssetInfoGeneric* AddAsset(asset_type_t type, std::string name, void* asset, std::vector<std::string>& scriptStrings, std::vector<XAssetInfoGeneric*>& dependencies) = 0;
    virtual XAssetInfoGeneric* GetAsset(asset_type_t type, std::string name) const = 0;
    virtual const std::string& GetAssetTypeName(asset_type_t assetType) const = 0;

    virtual void InitPoolStatic(asset_type_t type, size_t capacity) = 0;
    virtual void InitPoolDynamic(asset_type_t type) = 0;

    virtual iterator begin() const = 0;
    virtual iterator end() const = 0;
};
