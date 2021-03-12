#pragma once
#include <string>

#include "Utils/ClassUtils.h"
#include "AssetLoadingContext.h"
#include "Zone/Zone.h"

class IAssetLoader
{
public:
    IAssetLoader() = default;
    virtual ~IAssetLoader() = default;
    IAssetLoader(const IAssetLoader& other) = default;
    IAssetLoader(IAssetLoader&& other) noexcept = default;
    IAssetLoader& operator=(const IAssetLoader& other) = default;
    IAssetLoader& operator=(IAssetLoader&& other) noexcept = default;

    _NODISCARD virtual bool SupportsGame(const std::string& gameName) const = 0;
    _NODISCARD virtual std::unique_ptr<Zone> CreateZoneForDefinition(AssetLoadingContext& context) const = 0;
};
