#pragma once

#include "XAssetInfo.h"
#include "Zone/Zone.h"
#include "Zone/ZoneTypes.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Zone;
class IndirectAssetReference;
class XAssetInfoGeneric;

class ZoneAssetPools
{
public:
    using iterator = std::vector<XAssetInfoGeneric*>::const_iterator;

    explicit ZoneAssetPools(Zone* zone);
    virtual ~ZoneAssetPools() = default;
    ZoneAssetPools(const ZoneAssetPools& other) = delete;
    ZoneAssetPools(ZoneAssetPools&& other) noexcept = default;
    ZoneAssetPools& operator=(const ZoneAssetPools& other) = delete;
    ZoneAssetPools& operator=(ZoneAssetPools&& other) noexcept = default;

    XAssetInfoGeneric* AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo);
    XAssetInfoGeneric* AddAsset(asset_type_t type,
                                std::string name,
                                void* asset,
                                std::vector<XAssetInfoGeneric*> dependencies,
                                std::vector<scr_string_t> usedScriptStrings,
                                std::vector<IndirectAssetReference> indirectAssetReferences);
    [[nodiscard]] virtual XAssetInfoGeneric* GetAsset(asset_type_t type, const std::string& name) const = 0;
    [[nodiscard]] virtual XAssetInfoGeneric* GetAssetOrAssetReference(asset_type_t type, const std::string& name) const;

    [[nodiscard]] size_t GetTotalAssetCount() const;

    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;

    static std::unique_ptr<ZoneAssetPools> CreateForGame(GameId game, Zone* zone, zone_priority_t priority);

protected:
    virtual XAssetInfoGeneric* AddAssetToPool(std::unique_ptr<XAssetInfoGeneric> xAssetInfo) = 0;

    Zone* m_zone;
    std::vector<XAssetInfoGeneric*> m_assets_in_order;
};
