#pragma once

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cassert>

#include "AssetPool.h"

template<typename T>
class GlobalAssetPool
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

    static std::vector<LinkedAssetPool*> m_linked_asset_pools;
    static std::map<std::string, GameAssetPoolEntry> m_assets;

    static void SortLinkedAssetPools()
    {
        std::sort(m_linked_asset_pools.begin(), m_linked_asset_pools.end(), [](const LinkedAssetPool* a, const LinkedAssetPool* b) -> bool
        {
            return a->m_priority < b->m_priority;
        });
    }

    static bool ReplaceAssetPoolEntry(GameAssetPoolEntry& assetEntry)
    {
        int occurrences = 0;

        for(auto linkedAssetPool : m_linked_asset_pools)
        {
            XAssetInfo<T>* foundAsset = linkedAssetPool->m_asset_pool->GetAsset(assetEntry.m_asset->m_name);

            if(foundAsset != nullptr)
            {
                if(++occurrences == 1)
                {
                    assetEntry.m_asset = foundAsset;
                    assetEntry.m_duplicate = false;
                    assetEntry.m_asset_pool = linkedAssetPool;
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

        if(existingAsset == m_assets.end())
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

            if(existingEntry.m_asset_pool->m_priority < link->m_priority)
            {
                existingEntry.m_asset_pool = link;
                existingEntry.m_asset = asset;
            }
        }
    }

public:
    static void LinkAssetPool(AssetPool<T>* assetPool, const int priority)
    {
        auto* newLink = new LinkedAssetPool();
        newLink->m_asset_pool = assetPool;
        newLink->m_priority = priority;

        m_linked_asset_pools.push_back(newLink);
        SortLinkedAssetPools();

        for(auto asset : *assetPool)
        {
            LinkAsset(newLink, asset);
        }
    }

    static void LinkAsset(AssetPool<T>* assetPool, XAssetInfo<T>* asset)
    {
        LinkedAssetPool* link = nullptr;

        for(auto existingLink : m_linked_asset_pools)
        {
            if(existingLink->m_asset_pool == assetPool)
            {
                link = existingLink;
                break;
            }
        }

        assert(link != nullptr);
        if(link == nullptr)
            return;

        LinkAsset(link, asset);
    }

    static void UnlinkAssetPool(AssetPool<T>* assetPool)
    {
        auto iLinkEntry = m_linked_asset_pools.begin();

        for(; iLinkEntry != m_linked_asset_pools.end(); ++iLinkEntry)
        {
            LinkedAssetPool* linkEntry = *iLinkEntry;
            if(linkEntry->m_asset_pool == assetPool)
            {
                break;
            }
        }

        assert(iLinkEntry != m_linked_asset_pools.end());
        if(iLinkEntry == m_linked_asset_pools.end())
            return;

        m_linked_asset_pools.erase(iLinkEntry);

        for(auto iAssetEntry = m_assets.begin(); iAssetEntry != m_assets.end(); ++iAssetEntry)
        {
            auto& assetEntry = *iAssetEntry;

            if(assetEntry.second.m_duplicate && ReplaceAssetPoolEntry(assetEntry.second))
                continue;

            iAssetEntry = m_assets.erase(iAssetEntry);
        }
    }
};

template<typename T>
std::vector<typename GlobalAssetPool<T>::LinkedAssetPool*> GlobalAssetPool<T>::m_linked_asset_pools = std::vector<LinkedAssetPool*>();

template<typename T>
std::map<std::string, typename GlobalAssetPool<T>::GameAssetPoolEntry> GlobalAssetPool<T>::m_assets = std::map<std::string, GameAssetPoolEntry>();