#pragma once

#include "AssetPool.h"
#include "GlobalAssetPool.h"
#include "XAssetInfo.h"

#include <vector>

template<typename T> class AssetPoolDynamic final : public AssetPool<T>
{
    using AssetPool<T>::m_asset_lookup;

    std::vector<std::unique_ptr<XAssetInfo<T>>> m_assets;

public:
    explicit AssetPoolDynamic(const zone_priority_t priority)
    {
        GlobalAssetPool<T>::LinkAssetPool(this, priority);
    }

    AssetPoolDynamic(AssetPoolDynamic<T>&) = delete;
    AssetPoolDynamic(AssetPoolDynamic<T>&&) = delete;
    AssetPoolDynamic<T>& operator=(AssetPoolDynamic<T>&) = delete;
    AssetPoolDynamic<T>& operator=(AssetPoolDynamic<T>&&) = default;

    ~AssetPoolDynamic() override
    {
        GlobalAssetPool<T>::UnlinkAssetPool(this);

        m_assets.clear();
        m_asset_lookup.clear();
    }

    XAssetInfo<T>* AddAsset(std::unique_ptr<XAssetInfo<T>> xAssetInfo) override
    {
        const auto normalizedName = XAssetInfo<T>::NormalizeAssetName(xAssetInfo->m_name);

        auto* pAssetInfo = xAssetInfo.get();
        m_asset_lookup[normalizedName] = pAssetInfo;
        m_assets.emplace_back(std::move(xAssetInfo));

        GlobalAssetPool<T>::LinkAsset(this, normalizedName, pAssetInfo);

        return pAssetInfo;
    }
};
