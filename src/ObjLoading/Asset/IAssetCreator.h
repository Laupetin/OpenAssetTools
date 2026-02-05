#pragma once

#include "AssetCreationContext.h"
#include "AssetCreationResult.h"
#include "Game/IAsset.h"
#include "Pool/XAssetInfo.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/ZoneTypes.h"

#include <optional>
#include <string>

class AssetCreationContext;

class IAssetCreator
{
public:
    IAssetCreator() = default;
    virtual ~IAssetCreator() = default;
    IAssetCreator(const IAssetCreator& other) = default;
    IAssetCreator(IAssetCreator&& other) noexcept = default;
    IAssetCreator& operator=(const IAssetCreator& other) = default;
    IAssetCreator& operator=(IAssetCreator&& other) noexcept = default;

    [[nodiscard]] virtual std::optional<asset_type_t> GetHandlingAssetType() const = 0;
    virtual AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) = 0;

    virtual void FinalizeZone(AssetCreationContext& context) {}
};

class ISubAssetCreator
{
public:
    ISubAssetCreator() = default;
    virtual ~ISubAssetCreator() = default;
    ISubAssetCreator(const ISubAssetCreator& other) = default;
    ISubAssetCreator(ISubAssetCreator&& other) noexcept = default;
    ISubAssetCreator& operator=(const ISubAssetCreator& other) = default;
    ISubAssetCreator& operator=(ISubAssetCreator&& other) noexcept = default;

    [[nodiscard]] virtual std::optional<asset_type_t> GetHandlingSubAssetType() const = 0;
    virtual AssetCreationResult CreateSubAsset(const std::string& subAssetName, AssetCreationContext& context) = 0;

    virtual void FinalizeZone(AssetCreationContext& context) {}
};

template<AssetDefinition Asset_t> class AssetCreator : public IAssetCreator
{
public:
    [[nodiscard]] std::optional<asset_type_t> GetHandlingAssetType() const override
    {
        return Asset_t::EnumEntry;
    }
};

template<SubAssetDefinition SubAsset_t> class SubAssetCreator : public ISubAssetCreator
{
public:
    [[nodiscard]] std::optional<asset_type_t> GetHandlingSubAssetType() const override
    {
        return SubAsset_t::EnumEntry;
    }
};
