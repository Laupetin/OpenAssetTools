#pragma once
#include <vector>
#include <string>
#include <cstddef>

#include "Utils/ClassUtils.h"
#include "XAssetInfo.h"
#include "Zone/ZoneTypes.h"
#include "Zone/Zone.h"

class Zone;
class XAssetInfoGeneric;

class ZoneAssetPools
{
protected:
    Zone* m_zone;
    std::vector<XAssetInfoGeneric*> m_assets_in_order;

    virtual XAssetInfoGeneric* AddAssetToPool(asset_type_t type, std::string name, void* asset, std::vector<XAssetInfoGeneric*>& dependencies) = 0;

public:
    using iterator = std::vector<XAssetInfoGeneric*>::const_iterator;

    explicit ZoneAssetPools(Zone* zone);
    virtual ~ZoneAssetPools() = default;

    XAssetInfoGeneric* AddAsset(asset_type_t type, std::string name, void* asset, std::vector<XAssetInfoGeneric*>& dependencies);
    virtual XAssetInfoGeneric* GetAsset(asset_type_t type, std::string name) const = 0;
    virtual const char* GetAssetTypeName(asset_type_t assetType) const = 0;

    virtual void InitPoolStatic(asset_type_t type, size_t capacity) = 0;
    virtual void InitPoolDynamic(asset_type_t type) = 0;

    _NODISCARD size_t GetTotalAssetCount() const;

    iterator begin() const;
    iterator end() const;
};
