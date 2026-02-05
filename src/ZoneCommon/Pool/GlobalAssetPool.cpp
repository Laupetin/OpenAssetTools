#include "GlobalAssetPool.h"

#include <algorithm>
#include <cassert>

void GlobalAssetPool::LinkAssetPool(AssetPool* assetPool, const zone_priority_t priority)
{
    auto newLink = std::make_unique<LinkedAssetPool>();
    newLink->m_asset_pool = assetPool;
    newLink->m_priority = priority;

    auto* newLinkPtr = newLink.get();
    m_linked_asset_pools.emplace_back(std::move(newLink));
    SortLinkedAssetPools();

    for (auto* asset : *assetPool)
    {
        const auto normalizedAssetName = XAssetInfoGeneric::NormalizeAssetName(asset->m_name);
        LinkAsset(newLinkPtr, normalizedAssetName, asset);
    }
}

void GlobalAssetPool::LinkAsset(const AssetPool* assetPool, const std::string& normalizedAssetName, XAssetInfoGeneric* asset)
{
    LinkedAssetPool* link = nullptr;

    for (const auto& existingLink : m_linked_asset_pools)
    {
        if (existingLink->m_asset_pool == assetPool)
        {
            link = existingLink.get();
            break;
        }
    }

    assert(link != nullptr);
    if (link == nullptr)
        return;

    LinkAsset(link, normalizedAssetName, asset);
}

void GlobalAssetPool::UnlinkAssetPool(const AssetPool* assetPool)
{
    auto iLinkEntry = m_linked_asset_pools.begin();

    for (; iLinkEntry != m_linked_asset_pools.end(); ++iLinkEntry)
    {
        const auto* linkEntry = iLinkEntry->get();
        if (linkEntry->m_asset_pool == assetPool)
            break;
    }

    assert(iLinkEntry != m_linked_asset_pools.end());
    if (iLinkEntry == m_linked_asset_pools.end())
        return;

    const auto assetPoolToUnlink = std::move(*iLinkEntry);
    m_linked_asset_pools.erase(iLinkEntry);

    for (auto iAssetEntry = m_assets.begin(); iAssetEntry != m_assets.end();)
    {
        auto& assetEntry = *iAssetEntry;

        if (assetEntry.second.m_asset_pool != assetPoolToUnlink.get())
        {
            ++iAssetEntry;
            continue;
        }

        if (assetEntry.second.m_duplicate && ReplaceAssetPoolEntry(assetEntry.second))
        {
            ++iAssetEntry;
            continue;
        }

        iAssetEntry = m_assets.erase(iAssetEntry);
    }
}

XAssetInfoGeneric* GlobalAssetPool::GetAsset(const std::string& name)
{
    const auto normalizedName = XAssetInfoGeneric::NormalizeAssetName(name);
    const auto foundEntry = m_assets.find(normalizedName);
    if (foundEntry == m_assets.end())
        return nullptr;

    return foundEntry->second.m_asset;
}

void GlobalAssetPool::SortLinkedAssetPools()
{
    std::ranges::sort(m_linked_asset_pools,
                      [](const std::unique_ptr<LinkedAssetPool>& a, const std::unique_ptr<LinkedAssetPool>& b) -> bool
                      {
                          return a->m_priority < b->m_priority;
                      });
}

bool GlobalAssetPool::ReplaceAssetPoolEntry(GameAssetPoolEntry& assetEntry) const
{
    int occurrences = 0;

    for (const auto& linkedAssetPool : m_linked_asset_pools)
    {
        auto* foundAsset = linkedAssetPool->m_asset_pool->GetAsset(assetEntry.m_asset->m_name);

        if (foundAsset != nullptr)
        {
            if (++occurrences == 1)
            {
                assetEntry.m_asset = foundAsset;
                assetEntry.m_duplicate = false;
                assetEntry.m_asset_pool = linkedAssetPool.get();
            }
            else
            {
                assetEntry.m_duplicate = true;
                break;
            }
        }
    }

    return occurrences > 0;
}

void GlobalAssetPool::LinkAsset(LinkedAssetPool* link, const std::string& normalizedAssetName, XAssetInfoGeneric* asset)
{
    const auto existingAsset = m_assets.find(normalizedAssetName);

    if (existingAsset == m_assets.end())
    {
        const GameAssetPoolEntry entry{
            .m_asset = asset,
            .m_asset_pool = link,
            .m_duplicate = false,
        };

        m_assets[normalizedAssetName] = entry;
    }
    else
    {
        auto& existingEntry = existingAsset->second;

        existingEntry.m_duplicate = true;

        if (existingEntry.m_asset_pool->m_priority < link->m_priority)
        {
            existingEntry.m_asset_pool = link;
            existingEntry.m_asset = asset;
        }
    }
}

GameGlobalAssetPools::GameGlobalAssetPools(const GameId gameId)
{
    const auto assetTypeCount = IGame::GetGameById(gameId)->GetAssetTypeCount();
    m_global_asset_pools.resize(assetTypeCount);

    for (auto assetType = 0u; assetType < assetTypeCount; assetType++)
    {
        m_global_asset_pools[assetType] = std::make_unique<GlobalAssetPool>();
    }
}

void GameGlobalAssetPools::LinkAssetPool(const asset_type_t assetType, AssetPool* assetPool, const zone_priority_t priority) const
{
    assert(assetType < m_global_asset_pools.size());

    m_global_asset_pools[assetType]->LinkAssetPool(assetPool, priority);
}

void GameGlobalAssetPools::LinkAsset(const asset_type_t assetType,
                                     const AssetPool* assetPool,
                                     const std::string& normalizedAssetName,
                                     XAssetInfoGeneric* asset) const
{
    assert(assetType < m_global_asset_pools.size());

    m_global_asset_pools[assetType]->LinkAsset(assetPool, normalizedAssetName, asset);
}

void GameGlobalAssetPools::UnlinkAssetPool(const asset_type_t assetType, const AssetPool* assetPool) const
{
    assert(assetType < m_global_asset_pools.size());

    m_global_asset_pools[assetType]->UnlinkAssetPool(assetPool);
}

XAssetInfoGeneric* GameGlobalAssetPools::GetAsset(const asset_type_t assetType, const std::string& name) const
{
    assert(assetType < m_global_asset_pools.size());

    return m_global_asset_pools[assetType]->GetAsset(name);
}

GameGlobalAssetPools* GameGlobalAssetPools::GetGlobalPoolsForGame(GameId gameId)
{
    static GameGlobalAssetPools* globalAssetPools[static_cast<unsigned>(GameId::COUNT)]{
        new GameGlobalAssetPools(GameId::IW3),
        new GameGlobalAssetPools(GameId::IW4),
        new GameGlobalAssetPools(GameId::IW5),
        new GameGlobalAssetPools(GameId::T5),
        new GameGlobalAssetPools(GameId::T6),
    };

    assert(static_cast<unsigned>(gameId) < static_cast<unsigned>(GameId::COUNT));
    auto* result = globalAssetPools[static_cast<unsigned>(gameId)];
    assert(result);

    return result;
}
