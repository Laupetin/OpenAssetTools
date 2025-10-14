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

    [[nodiscard]] virtual size_t GetProgressTotalCount(const AssetPool<T>& pool) const = 0;
    virtual void DumpPool(AssetDumpingContext& context, const AssetPool<T>& pool) = 0;
};
