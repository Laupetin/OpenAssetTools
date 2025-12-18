#pragma once

#include "AssetCreationContext.h"
#include "AssetCreationResult.h"
#include "Game/IAsset.h"
#include "IAssetCreator.h"
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
    virtual AssetCreationResult CreateDefaultAsset(const std::string& assetName, AssetCreationContext& context) const = 0;
};

template<typename AssetType> class DefaultAssetCreator : public IDefaultAssetCreator
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    DefaultAssetCreator(MemoryManager& memory)
        : m_memory(memory)
    {
    }

    [[nodiscard]] asset_type_t GetHandlingAssetType() const override
    {
        return AssetType::EnumEntry;
    }

    AssetCreationResult CreateDefaultAsset(const std::string& assetName, AssetCreationContext& context) const override
    {
        auto* asset = m_memory.Alloc<typename AssetType::Type>();
        AssetName<AssetType>(*asset) = m_memory.Dup(assetName.c_str());

        return AssetCreationResult::Success(context.AddAsset<AssetType>(assetName, asset));
    }

private:
    MemoryManager& m_memory;
};
