#pragma once

#include "Zone/ZoneTypes.h"

#include <stdexcept>
#include <type_traits>

template<asset_type_t AssetTypeEnum, class AssetType> class Asset
{
public:
    static constexpr bool IS_ASSET = true;
    static constexpr auto EnumEntry = AssetTypeEnum;
    using Type = AssetType;
};

template<asset_type_t AssetTypeEnum, class AssetType> class SubAsset
{
public:
    static constexpr bool IS_ASSET = false;
    static constexpr auto EnumEntry = AssetTypeEnum;
    using Type = AssetType;
};

template<typename T>
concept HasAssetEnumEntry = std::is_integral_v<decltype(T::EnumEntry)>;

template<typename T>
concept HasAssetType = std::is_class_v<typename T::Type>;

template<typename T>
concept AssetOrSubAssetDefinition = HasAssetEnumEntry<T> && HasAssetType<T>;

template<typename T>
concept AssetDefinition = AssetOrSubAssetDefinition<T> && T::IS_ASSET;

template<typename T>
concept SubAssetDefinition = AssetOrSubAssetDefinition<T> && !T::IS_ASSET;

template<AssetOrSubAssetDefinition> struct AssetNameAccessor
{
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
        static constexpr bool IS_SINGLETON = true;                                                                                                             \
        using RETURN_TYPE = const char* const&;                                                                                                                \
                                                                                                                                                               \
        static RETURN_TYPE GetAssetName(assetType::Type& asset)                                                                                                \
        {                                                                                                                                                      \
            static const char* NAME = singletonName;                                                                                                           \
            return NAME;                                                                                                                                       \
        }                                                                                                                                                      \
    }

template<AssetDefinition Asset_t> AssetNameAccessor<Asset_t>::RETURN_TYPE AssetName(typename Asset_t::Type& asset)
{
    return AssetNameAccessor<Asset_t>::GetAssetName(asset);
}
