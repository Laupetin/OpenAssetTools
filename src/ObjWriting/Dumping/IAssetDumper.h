#pragma once

#include "AssetDumpingContext.h"
#include "Zone/ZoneTypes.h"

#include <optional>

class IAssetDumper
{
public:
    IAssetDumper() = default;
    virtual ~IAssetDumper() = default;
    IAssetDumper(const IAssetDumper& other) = default;
    IAssetDumper(IAssetDumper&& other) noexcept = default;
    IAssetDumper& operator=(const IAssetDumper& other) = default;
    IAssetDumper& operator=(IAssetDumper&& other) noexcept = default;

    [[nodiscard]] virtual std::optional<asset_type_t> GetHandlingAssetType() const
    {
        return std::nullopt;
    }

    [[nodiscard]] virtual size_t GetProgressTotalCount(AssetDumpingContext& context) const = 0;
    virtual void Dump(AssetDumpingContext& context) = 0;
};
