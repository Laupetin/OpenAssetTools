#pragma once

#include "AssetDumpingContext.h"

class IAssetDumper
{
public:
    IAssetDumper() = default;
    virtual ~IAssetDumper() = default;
    IAssetDumper(const IAssetDumper& other) = default;
    IAssetDumper(IAssetDumper&& other) noexcept = default;
    IAssetDumper& operator=(const IAssetDumper& other) = default;
    IAssetDumper& operator=(IAssetDumper&& other) noexcept = default;

    [[nodiscard]] virtual size_t GetProgressTotalCount() const = 0;
    virtual void Dump(AssetDumpingContext& context) = 0;
};
