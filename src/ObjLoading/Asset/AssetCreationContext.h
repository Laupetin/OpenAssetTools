#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "AssetRegistration.h"
#include "Game/IAsset.h"
#include "Pool/XAssetInfo.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/ZoneTypes.h"

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

class AssetCreatorCollection;

class IgnoredAssetLookup
{
public:
    IgnoredAssetLookup();
    explicit IgnoredAssetLookup(const AssetList& assetList);

    [[nodiscard]] bool IsAssetIgnored(asset_type_t assetType, const std::string& name) const;

    std::unordered_multimap<std::string, asset_type_t> m_ignored_asset_lookup;
};

class AssetCreationContext : public ZoneAssetCreationStateContainer
{
public:
    AssetCreationContext(Zone& zone, const AssetCreatorCollection* creators, const IgnoredAssetLookup* ignoredAssetLookup);

    template<typename AssetType> XAssetInfo<typename AssetType::Type>* AddAsset(AssetRegistration<AssetType> registration)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(AddAssetGeneric(std::move(registration)));
    }

    template<typename AssetType> XAssetInfo<typename AssetType::Type>* AddAsset(std::string assetName, typename AssetType::Type* asset)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(AddAssetGeneric(AssetRegistration<AssetType>(std::move(assetName), asset)));
    }

    XAssetInfoGeneric* AddAssetGeneric(GenericAssetRegistration registration) const;

    template<typename AssetType> XAssetInfo<typename AssetType::Type>* LoadDependency(const std::string& assetName)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(LoadDependencyGeneric(AssetType::EnumEntry, assetName));
    }

    XAssetInfoGeneric* LoadDependencyGeneric(asset_type_t assetType, const std::string& assetName, bool required = true);

    template<typename AssetType> IndirectAssetReference LoadIndirectAssetReference(const std::string& assetName)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return LoadIndirectAssetReferenceGeneric(AssetType::EnumEntry, assetName);
    }

    IndirectAssetReference LoadIndirectAssetReferenceGeneric(asset_type_t assetType, const std::string& assetName);

    /**
     * \brief Loads an asset dependency like \c LoadDependency but guarantees that the returned asset is not a reference.
     * If normally a reference would be created, the actual asset is loaded but a reference is added to the zone.
     * \tparam AssetType The type of the asset
     * \param assetName The name of the asset
     * \return XAssetInfo of the asset that is guaranteed to not be a reference or \c nullptr
     */
    template<typename AssetType> XAssetInfo<typename AssetType::Type>* ForceLoadDependency(const std::string& assetName)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(ForceLoadDependencyGeneric(AssetType::EnumEntry, assetName));
    }

    XAssetInfoGeneric* ForceLoadDependencyGeneric(asset_type_t assetType, const std::string& assetName);

private:
    [[nodiscard]] XAssetInfoGeneric* LoadDefaultAssetDependency(asset_type_t assetType, const std::string& assetName);

    Zone& m_zone;
    std::unique_ptr<ZoneAssetPools> m_forced_asset_pools;
    const AssetCreatorCollection* m_creators;
    const IgnoredAssetLookup* m_ignored_asset_lookup;

    unsigned m_forced_load_depth;
};

#include "AssetCreatorCollection.h"
