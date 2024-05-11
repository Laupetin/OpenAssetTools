#pragma once

#include "AssetPool.h"
#include "GlobalAssetPool.h"
#include "XAssetInfo.h"

#include <cstring>

template<typename T> class AssetPoolDynamic final : public AssetPool<T>
{
    using AssetPool<T>::m_asset_lookup;

    std::vector<std::unique_ptr<XAssetInfo<T>>> m_assets;
    asset_type_t m_type;

public:
    AssetPoolDynamic(const int priority, const asset_type_t type)
    {
        GlobalAssetPool<T>::LinkAssetPool(this, priority);
        m_type = type;
    }

    AssetPoolDynamic(AssetPoolDynamic<T>&) = delete;
    AssetPoolDynamic(AssetPoolDynamic<T>&&) = delete;
    AssetPoolDynamic<T>& operator=(AssetPoolDynamic<T>&) = delete;
    AssetPoolDynamic<T>& operator=(AssetPoolDynamic<T>&&) = default;

    ~AssetPoolDynamic() override
    {
        GlobalAssetPool<T>::UnlinkAssetPool(this);

        for (auto& entry : m_assets)
        {
            delete entry->Asset();
        }

        m_assets.clear();
        m_asset_lookup.clear();
    }

    XAssetInfo<T>* AddAsset(std::unique_ptr<XAssetInfo<T>> xAssetInfo) override
    {
        const auto normalizedName = XAssetInfo<T>::NormalizeAssetName(xAssetInfo->m_name);

        T* newAsset = new T();
        memcpy(newAsset, xAssetInfo->Asset(), sizeof(T));
        xAssetInfo->m_ptr = newAsset;

        auto* pAssetInfo = xAssetInfo.get();
        m_asset_lookup[normalizedName] = pAssetInfo;
        m_assets.emplace_back(std::move(xAssetInfo));

        GlobalAssetPool<T>::LinkAsset(this, normalizedName, pAssetInfo);

        return pAssetInfo;
    }
};
