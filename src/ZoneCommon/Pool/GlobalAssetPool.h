#pragma once

#include "AssetPool.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

template<typename T> class GlobalAssetPool
{
    struct LinkedAssetPool
    {
        AssetPool<T>* m_asset_pool;
        int m_priority;
    };

    struct GameAssetPoolEntry
    {
        XAssetInfo<T>* m_asset;
        bool m_duplicate;
        LinkedAssetPool* m_asset_pool;
    };

    static std::vector<std::unique_ptr<LinkedAssetPool>> m_linked_asset_pools;
    static std::unordered_map<std::string, GameAssetPoolEntry> m_assets;

    static void SortLinkedAssetPools()
    {
        std::sort(m_linked_asset_pools.begin(),
                  m_linked_asset_pools.end(),
                  [](const std::unique_ptr<LinkedAssetPool>& a, const std::unique_ptr<LinkedAssetPool>& b) -> bool
                  {
                      return a->m_priority < b->m_priority;
                  });
    }

    static bool ReplaceAssetPoolEntry(GameAssetPoolEntry& assetEntry)
    {
        int occurrences = 0;

        for (const auto& linkedAssetPool : m_linked_asset_pools)
        {
            XAssetInfo<T>* foundAsset = linkedAssetPool->m_asset_pool->GetAsset(assetEntry.m_asset->m_name);

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

    static void LinkAsset(LinkedAssetPool* link, XAssetInfo<T>* asset)
    {
        std::string assetName = std::string(asset->m_name);

        auto existingAsset = m_assets.find(assetName);

        if (existingAsset == m_assets.end())
        {
            GameAssetPoolEntry entry{};
            entry.m_asset = asset;
            entry.m_asset_pool = link;
            entry.m_duplicate = false;

            m_assets[assetName] = entry;
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

public:
    static void LinkAssetPool(AssetPool<T>* assetPool, const int priority)
    {
        auto newLink = std::make_unique<LinkedAssetPool>();
        newLink->m_asset_pool = assetPool;
        newLink->m_priority = priority;

        auto* newLinkPtr = newLink.get();
        m_linked_asset_pools.emplace_back(std::move(newLink));
        SortLinkedAssetPools();

        for (auto asset : *assetPool)
        {
            LinkAsset(newLinkPtr, asset);
        }
    }

    static void LinkAsset(AssetPool<T>* assetPool, XAssetInfo<T>* asset)
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

        LinkAsset(link, asset);
    }

    static void UnlinkAssetPool(AssetPool<T>* assetPool)
    {
        auto iLinkEntry = m_linked_asset_pools.begin();

        for (; iLinkEntry != m_linked_asset_pools.end(); ++iLinkEntry)
        {
            LinkedAssetPool* linkEntry = iLinkEntry->get();
            if (linkEntry->m_asset_pool == assetPool)
            {
                break;
            }
        }

        assert(iLinkEntry != m_linked_asset_pools.end());
        if (iLinkEntry == m_linked_asset_pools.end())
            return;

        auto assetPoolToUnlink = std::move(*iLinkEntry);
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

    static XAssetInfo<T>* GetAssetByName(const std::string& name)
    {
        const auto foundEntry = m_assets.find(name);
        if (foundEntry == m_assets.end())
            return nullptr;

        return foundEntry->second.m_asset;
    }
};

template<typename T>
std::vector<std::unique_ptr<typename GlobalAssetPool<T>::LinkedAssetPool>> GlobalAssetPool<T>::m_linked_asset_pools =
    std::vector<std::unique_ptr<LinkedAssetPool>>();

template<typename T>
std::unordered_map<std::string, typename GlobalAssetPool<T>::GameAssetPoolEntry> GlobalAssetPool<T>::m_assets =
    std::unordered_map<std::string, GameAssetPoolEntry>();
