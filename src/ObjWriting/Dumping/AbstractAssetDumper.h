#pragma once

#include "IAssetDumper.h"

template<class T> class AbstractAssetDumper : public IAssetDumper<T>
{
protected:
    virtual bool ShouldDump(XAssetInfo<T>* asset)
    {
        return true;
    }

    virtual void DumpAsset(AssetDumpingContext& context, XAssetInfo<T>* asset) = 0;

public:
    void DumpPool(AssetDumpingContext& context, AssetPool<T>* pool) override
    {
        for (auto assetInfo : *pool)
        {
            if (assetInfo->m_name[0] == ',' || !ShouldDump(assetInfo))
            {
                continue;
            }

            DumpAsset(context, assetInfo);
        }
    }
};
