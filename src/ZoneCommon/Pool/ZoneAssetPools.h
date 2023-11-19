#pragma once
#include "Utils/ClassUtils.h"
#include "XAssetInfo.h"
#include "Zone/Zone.h"
#include "Zone/ZoneTypes.h"

#include <cstddef>
#include <string>
#include <vector>

class Zone;
class XAssetInfoGeneric;

class ZoneAssetPools
{
protected:
    Zone* m_zone;
    std::vector<XAssetInfoGeneric*> m_assets_in_order;

    virtual XAssetInfoGeneric* AddAssetToPool(asset_type_t type,
                                              std::string name,
                                              void* asset,
                                              std::vector<XAssetInfoGeneric*> dependencies,
                                              std::vector<scr_string_t> usedScriptStrings,
                                              Zone* zone) = 0;

public:
    using iterator = std::vector<XAssetInfoGeneric*>::const_iterator;

    explicit ZoneAssetPools(Zone* zone);
    virtual ~ZoneAssetPools() = default;
    ZoneAssetPools(const ZoneAssetPools& other) = delete;
    ZoneAssetPools(ZoneAssetPools&& other) noexcept = default;
    ZoneAssetPools& operator=(const ZoneAssetPools& other) = delete;
    ZoneAssetPools& operator=(ZoneAssetPools&& other) noexcept = default;

    XAssetInfoGeneric*
        AddAsset(asset_type_t type, std::string name, void* asset, std::vector<XAssetInfoGeneric*> dependencies, std::vector<scr_string_t> usedScriptStrings);
    XAssetInfoGeneric* AddAsset(asset_type_t type,
                                std::string name,
                                void* asset,
                                std::vector<XAssetInfoGeneric*> dependencies,
                                std::vector<scr_string_t> usedScriptStrings,
                                Zone* zone);
    _NODISCARD virtual XAssetInfoGeneric* GetAsset(asset_type_t type, std::string name) const = 0;
    _NODISCARD virtual asset_type_t GetAssetTypeCount() const = 0;
    _NODISCARD virtual const char* GetAssetTypeName(asset_type_t assetType) const = 0;

    virtual void InitPoolStatic(asset_type_t type, size_t capacity) = 0;
    virtual void InitPoolDynamic(asset_type_t type) = 0;

    _NODISCARD size_t GetTotalAssetCount() const;

    iterator begin() const;
    iterator end() const;
};
