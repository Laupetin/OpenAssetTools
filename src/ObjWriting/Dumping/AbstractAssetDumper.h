#pragma once

#include "IAssetDumper.h"

template<class T> class AbstractAssetDumper : public IAssetDumper<T>
{
public:
    [[nodiscard]] size_t GetProgressTotalCount(const AssetPool<T>& pool) const override
    {
        return pool.m_asset_lookup.size();
    }

    void DumpPool(AssetDumpingContext& context, const AssetPool<T>& pool) override
    {
        for (const auto* assetInfo : pool)
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
    virtual bool ShouldDump(const XAssetInfo<T>& asset)
    {
        return true;
    }

    virtual void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T>& asset) = 0;
};
