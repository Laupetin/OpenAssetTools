#pragma once

#include "AssetCreationContext.h"
#include "Game/IAsset.h"
#include "IAssetCreator.h"
#include "Zone/ZoneTypes.h"

#include <string>
#include <type_traits>

class AssetCreationResult;
class AssetCreationContext;

class IDefaultAssetCreator
{
public:
    IDefaultAssetCreator() = default;
    virtual ~IDefaultAssetCreator() = default;
    IDefaultAssetCreator(const IDefaultAssetCreator& other) = default;
    IDefaultAssetCreator(IDefaultAssetCreator&& other) noexcept = default;
    IDefaultAssetCreator& operator=(const IDefaultAssetCreator& other) = default;
    IDefaultAssetCreator& operator=(IDefaultAssetCreator&& other) noexcept = default;

    [[nodiscard]] virtual asset_type_t GetHandlingAssetType() const = 0;
    virtual AssetCreationResult CreateDefaultAsset(const std::string& assetName, AssetCreationContext& context) const = 0;
};

template<typename AssetType> class DefaultAssetCreator : public IDefaultAssetCreator
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    [[nodiscard]] asset_type_t GetHandlingAssetType() const override
    {
        return AssetType::EnumEntry;
    }
};
