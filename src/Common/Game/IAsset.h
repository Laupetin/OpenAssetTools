#pragma once

#include "Zone/ZoneTypes.h"

#include <stdexcept>
#include <type_traits>

struct IAssetBase
{
};

template<asset_type_t AssetTypeEnum, typename AssetType> class Asset : IAssetBase
{
public:
    static constexpr auto EnumEntry = AssetTypeEnum;
    using Type = AssetType;
};

template<typename AssetType> struct AssetNameAccessor
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    const char*& operator()(AssetType::Type& asset)
    {
        throw std::runtime_error();
    }
};

#define DEFINE_ASSET_NAME_ACCESSOR(assetType, nameProperty)                                                                                                    \
    template<> struct AssetNameAccessor<assetType>                                                                                                             \
    {                                                                                                                                                          \
    public:                                                                                                                                                    \
        static_assert(std::is_base_of_v<IAssetBase, assetType>);                                                                                               \
                                                                                                                                                               \
        const char*& operator()(assetType::Type& asset)                                                                                                        \
        {                                                                                                                                                      \
            return asset.nameProperty;                                                                                                                         \
        }                                                                                                                                                      \
    }
