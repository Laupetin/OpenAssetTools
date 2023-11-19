#pragma once

#include "AssetDumpingContext.h"
#include "Pool/AssetPool.h"

template<class T> class IAssetDumper
{
public:
    IAssetDumper() = default;
    virtual ~IAssetDumper() = default;
    IAssetDumper(const IAssetDumper& other) = default;
    IAssetDumper(IAssetDumper&& other) noexcept = default;
    IAssetDumper& operator=(const IAssetDumper& other) = default;
    IAssetDumper& operator=(IAssetDumper&& other) noexcept = default;

    virtual void DumpPool(AssetDumpingContext& context, AssetPool<T>* pool) = 0;
};
