#pragma once

#include "AssetPool.h"
#include "Zone/ZoneTypes.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct LinkedAssetPool
{
    AssetPool* m_asset_pool;
    zone_priority_t m_priority;
};

struct GameAssetPoolEntry
{
    XAssetInfoGeneric* m_asset;
    LinkedAssetPool* m_asset_pool;
    bool m_duplicate;
};

class GlobalAssetPool
{
public:
    void LinkAssetPool(AssetPool* assetPool, zone_priority_t priority);
    void LinkAsset(const AssetPool* assetPool, const std::string& normalizedAssetName, XAssetInfoGeneric* asset);
    void UnlinkAssetPool(const AssetPool* assetPool);

    XAssetInfoGeneric* GetAsset(const std::string& name);

private:
    void SortLinkedAssetPools();
    bool ReplaceAssetPoolEntry(GameAssetPoolEntry& assetEntry) const;
    void LinkAsset(LinkedAssetPool* link, const std::string& normalizedAssetName, XAssetInfoGeneric* asset);

    std::vector<std::unique_ptr<LinkedAssetPool>> m_linked_asset_pools;
    std::unordered_map<std::string, GameAssetPoolEntry> m_assets;
};

class GameGlobalAssetPools
{
public:
    explicit GameGlobalAssetPools(GameId gameId);

    void LinkAssetPool(asset_type_t assetType, AssetPool* assetPool, zone_priority_t priority) const;
    void LinkAsset(asset_type_t assetType, const AssetPool* assetPool, const std::string& normalizedAssetName, XAssetInfoGeneric* asset) const;
    void UnlinkAssetPool(asset_type_t assetType, const AssetPool* assetPool) const;

    [[nodiscard]] XAssetInfoGeneric* GetAsset(asset_type_t assetType, const std::string& name) const;

    template<AssetDefinition Asset_t> [[nodiscard]] XAssetInfo<typename Asset_t::Type>* GetAsset(const std::string& name) const
    {
        return reinterpret_cast<XAssetInfo<typename Asset_t::Type>*>(GetAsset(Asset_t::EnumEntry, name));
    }

    static GameGlobalAssetPools* GetGlobalPoolsForGame(GameId gameId);

private:
    std::vector<std::unique_ptr<GlobalAssetPool>> m_global_asset_pools;
};
