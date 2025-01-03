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
    virtual void FinalizeZone(AssetCreationContext& context){};
};

template<typename AssetType> class AssetCreator : public IAssetCreator
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    [[nodiscard]] std::optional<asset_type_t> GetHandlingAssetType() const override
    {
        return AssetType::EnumEntry;
    };
};
