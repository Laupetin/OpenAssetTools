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
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);
    // static constexpr bool IS_SINGLETON = false;
    // using RETURN_TYPE = const char*&;

    // static RETURN_TYPE GetAssetName(assetType::Type& asset)
    // {
    //     throw std::runtime_error("Not implemented");
    // }
};

#define DEFINE_ASSET_NAME_ACCESSOR(assetType, nameProperty)                                                                                                    \
    template<> struct AssetNameAccessor<assetType>                                                                                                             \
    {                                                                                                                                                          \
    public:                                                                                                                                                    \
        static_assert(std::is_base_of_v<IAssetBase, assetType>);                                                                                               \
        static constexpr bool IS_SINGLETON = false;                                                                                                            \
        using RETURN_TYPE = const char*&;                                                                                                                      \
                                                                                                                                                               \
        static RETURN_TYPE GetAssetName(assetType::Type& asset)                                                                                                \
        {                                                                                                                                                      \
            return asset.nameProperty;                                                                                                                         \
        }                                                                                                                                                      \
    }

#define DEFINE_ASSET_NAME_ACCESSOR_SINGLETON(assetType, singletonName)                                                                                         \
    template<> struct AssetNameAccessor<assetType>                                                                                                             \
    {                                                                                                                                                          \
    public:                                                                                                                                                    \
        static_assert(std::is_base_of_v<IAssetBase, assetType>);                                                                                               \
        static constexpr bool IS_SINGLETON = true;                                                                                                             \
        using RETURN_TYPE = const char* const&;                                                                                                                \
                                                                                                                                                               \
        static RETURN_TYPE GetAssetName(assetType::Type& asset)                                                                                                \
        {                                                                                                                                                      \
            static const char* NAME = singletonName;                                                                                                           \
            return NAME;                                                                                                                                       \
        }                                                                                                                                                      \
    }

template<typename AssetType> AssetNameAccessor<AssetType>::RETURN_TYPE AssetName(typename AssetType::Type& asset)
{
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);
    return AssetNameAccessor<AssetType>::GetAssetName(asset);
}
