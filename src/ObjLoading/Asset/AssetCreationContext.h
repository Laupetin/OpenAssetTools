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

    template<AssetDefinition Asset_t> XAssetInfo<typename Asset_t::Type>* AddAsset(AssetRegistration<Asset_t> registration)
    {
        return static_cast<XAssetInfo<typename Asset_t::Type>*>(AddAssetGeneric(std::move(registration)));
    }

    template<AssetDefinition Asset_t> XAssetInfo<typename Asset_t::Type>* AddAsset(std::string assetName, Asset_t::Type* asset)
    {
        return static_cast<XAssetInfo<typename Asset_t::Type>*>(AddAssetGeneric(AssetRegistration<Asset_t>(std::move(assetName), asset)));
    }

    XAssetInfoGeneric* AddAssetGeneric(GenericAssetRegistration registration) const;

    template<SubAssetDefinition SubAsset_t> XAssetInfo<typename SubAsset_t::Type>* AddSubAsset(AssetRegistration<SubAsset_t> registration)
    {
        return static_cast<XAssetInfo<typename SubAsset_t::Type>*>(AddSubAssetGeneric(std::move(registration)));
    }

    template<SubAssetDefinition SubAsset_t> XAssetInfo<typename SubAsset_t::Type>* AddSubAsset(std::string assetName, SubAsset_t::Type* asset)
    {
        return static_cast<XAssetInfo<typename SubAsset_t::Type>*>(AddSubAssetGeneric(AssetRegistration<SubAsset_t>(std::move(assetName), asset)));
    }

    XAssetInfoGeneric* AddSubAssetGeneric(GenericAssetRegistration registration) const;

    template<AssetDefinition Asset_t> XAssetInfo<typename Asset_t::Type>* LoadDependency(const std::string& assetName)
    {
        return static_cast<XAssetInfo<typename Asset_t::Type>*>(LoadDependencyGeneric(Asset_t::EnumEntry, assetName));
    }

    XAssetInfoGeneric* LoadDependencyGeneric(asset_type_t assetType, const std::string& assetName, bool required = true);

    template<SubAssetDefinition SubAsset_t> XAssetInfo<typename SubAsset_t::Type>* LoadSubAsset(const std::string& assetName)
    {
        return static_cast<XAssetInfo<typename SubAsset_t::Type>*>(LoadSubAssetGeneric(SubAsset_t::EnumEntry, assetName));
    }

    XAssetInfoGeneric* LoadSubAssetGeneric(asset_type_t subAssetType, const std::string& assetName);

    template<AssetDefinition Asset_t> IndirectAssetReference LoadIndirectAssetReference(const std::string& assetName)
    {
        return LoadIndirectAssetReferenceGeneric(Asset_t::EnumEntry, assetName);
    }

    IndirectAssetReference LoadIndirectAssetReferenceGeneric(asset_type_t assetType, const std::string& assetName);

    /**
     * \brief Loads an asset dependency like \c LoadDependency but guarantees that the returned asset is not a reference.
     * If normally a reference would be created, the actual asset is loaded but a reference is added to the zone.
     * \tparam Asset_t The type of the asset
     * \param assetName The name of the asset
     * \return XAssetInfo of the asset that is guaranteed to not be a reference or \c nullptr
     */
    template<AssetDefinition Asset_t> XAssetInfo<typename Asset_t::Type>* ForceLoadDependency(const std::string& assetName)
    {
        return static_cast<XAssetInfo<typename Asset_t::Type>*>(ForceLoadDependencyGeneric(Asset_t::EnumEntry, assetName));
    }

    XAssetInfoGeneric* ForceLoadDependencyGeneric(asset_type_t assetType, const std::string& assetName);

private:
    [[nodiscard]] XAssetInfoGeneric* LoadDefaultAssetDependency(asset_type_t assetType, const std::string& assetName);

    Zone& m_zone;
    std::unique_ptr<ZoneAssetPools> m_forced_asset_pools;
    std::vector<std::unique_ptr<AssetPool>> m_sub_asset_pools;
    const AssetCreatorCollection* m_creators;
    const IgnoredAssetLookup* m_ignored_asset_lookup;

    unsigned m_forced_load_depth;
};

#include "AssetCreatorCollection.h"
