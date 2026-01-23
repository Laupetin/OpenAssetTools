#pragma once

#include "Game/IAsset.h"
#include "IAssetDumper.h"
#include "Pool/AssetPool.h"

template<AssetDefinition Asset_t> class AbstractAssetDumper : public IAssetDumper
{
public:
    using AssetType_t = Asset_t;

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
    explicit AbstractAssetDumper(const AssetPool<typename Asset_t::Type>& pool)
        : m_pool(pool)
    {
    }

    virtual bool ShouldDump(const XAssetInfo<typename Asset_t::Type>& asset)
    {
        return true;
    }

    virtual void DumpAsset(AssetDumpingContext& context, const XAssetInfo<typename Asset_t::Type>& asset) = 0;

    const AssetPool<typename Asset_t::Type>& m_pool;
};

template<AssetDefinition Asset_t> class AbstractSingleProgressAssetDumper : public IAssetDumper
{
public:
    using AssetType_t = Asset_t;

    [[nodiscard]] size_t GetProgressTotalCount() const override
    {
        return m_pool.m_asset_lookup.empty() ? 0uz : 1uz;
    }

protected:
    explicit AbstractSingleProgressAssetDumper(const AssetPool<typename Asset_t::Type>& pool)
        : m_pool(pool)
    {
    }

    const AssetPool<typename Asset_t::Type>& m_pool;
};
