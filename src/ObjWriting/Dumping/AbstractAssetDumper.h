#pragma once

#include "Game/IAsset.h"
#include "IAssetDumper.h"
#include "Pool/AssetPool.h"

template<class AssetType> class AbstractAssetDumper : public IAssetDumper
{
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

public:
    using AssetType_t = AssetType;

    [[nodiscard]] size_t GetProgressTotalCount() const override
    {
        return m_pool.m_asset_lookup.size();
    }

    void Dump(AssetDumpingContext& context) override
    {
        for (const auto* assetInfo : m_pool)
        {
            if (assetInfo->m_name[0] == ',' || !ShouldDump(*assetInfo))
            {
                context.IncrementProgress();
                continue;
            }

            DumpAsset(context, *assetInfo);
            context.IncrementProgress();
        }
    }

protected:
    explicit AbstractAssetDumper(const AssetPool<typename AssetType::Type>& pool)
        : m_pool(pool)
    {
    }

    virtual bool ShouldDump(const XAssetInfo<typename AssetType::Type>& asset)
    {
        return true;
    }

    virtual void DumpAsset(AssetDumpingContext& context, const XAssetInfo<typename AssetType::Type>& asset) = 0;

    const AssetPool<typename AssetType::Type>& m_pool;
};

template<class AssetType> class AbstractSingleProgressAssetDumper : public IAssetDumper
{
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

public:
    using AssetType_t = AssetType;

    [[nodiscard]] size_t GetProgressTotalCount() const override
    {
        return m_pool.m_asset_lookup.empty() ? 0uz : 1uz;
    }

protected:
    explicit AbstractSingleProgressAssetDumper(const AssetPool<typename AssetType::Type>& pool)
        : m_pool(pool)
    {
    }

    const AssetPool<typename AssetType::Type>& m_pool;
};
