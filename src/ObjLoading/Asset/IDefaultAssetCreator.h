#pragma once

#include "Utils/MemoryManager.h"
#include "Zone/ZoneTypes.h"

#include <string>
#include <type_traits>

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
    virtual GenericAssetRegistration CreateDefaultAsset(const std::string& assetName, MemoryManager& memory) const = 0;
};

template<typename AssetType> class DefaultAssetCreator : public IDefaultAssetCreator
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    [[nodiscard]] asset_type_t GetHandlingAssetType() const override
    {
        return AssetType::EnumEntry;
    }

    GenericAssetRegistration CreateDefaultAsset(const std::string& assetName, MemoryManager& memory) const override
    {
        return CreateDefaultAssetInternal(assetName, memory);
    }

protected:
    virtual AssetRegistration<typename AssetType::Type> CreateDefaultAssetInternal(const std::string& assetName, MemoryManager& memory) const = 0;
};
