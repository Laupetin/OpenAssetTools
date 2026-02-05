#pragma once

#include "Game/IAsset.h"
#include "IAssetDumper.h"
#include "Pool/AssetPool.h"

template<AssetDefinition Asset_t> class AbstractAssetDumper : public IAssetDumper
{
public:
    using AssetType_t = Asset_t;

    [[nodiscard]] std::optional<asset_type_t> GetHandlingAssetType() const override
    {
        return Asset_t::EnumEntry;
    }

    [[nodiscard]] size_t GetProgressTotalCount(AssetDumpingContext& context) const override
    {
        return context.m_zone.m_pools.PoolAssets<Asset_t>().size();
    }

    void Dump(AssetDumpingContext& context) override
    {
        for (const auto* assetInfo : context.m_zone.m_pools.PoolAssets<Asset_t>())
        {
            if (assetInfo->IsReference() || !ShouldDump(*assetInfo))
            {
                context.IncrementProgress();
                continue;
            }

            DumpAsset(context, *assetInfo);
            context.IncrementProgress();
        }
    }

protected:
    virtual bool ShouldDump(const XAssetInfo<typename Asset_t::Type>& asset)
    {
        return true;
    }

    virtual void DumpAsset(AssetDumpingContext& context, const XAssetInfo<typename Asset_t::Type>& asset) = 0;
};

template<AssetDefinition Asset_t> class AbstractSingleProgressAssetDumper : public IAssetDumper
{
public:
    using AssetType_t = Asset_t;

    [[nodiscard]] std::optional<asset_type_t> GetHandlingAssetType() const override
    {
        return Asset_t::EnumEntry;
    }

    [[nodiscard]] size_t GetProgressTotalCount(AssetDumpingContext& context) const override
    {
        return context.m_zone.m_pools.PoolAssets<Asset_t>().empty() ? 0uz : 1uz;
    }
};
