#pragma once

#include "AssetLoading/IZoneAssetLoaderState.h"
#include "AssetRegistration.h"
#include "Game/IAsset.h"
#include "Pool/XAssetInfo.h"
#include "Zone/AssetList/AssetList.h"
#include "Zone/ZoneTypes.h"

#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
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

class AssetCreationContext
{
public:
    AssetCreationContext(Zone* zone, const AssetCreatorCollection* creators, const IgnoredAssetLookup* ignoredAssetLookup);

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

    XAssetInfoGeneric* LoadDependencyGeneric(asset_type_t assetType, const std::string& assetName);

    template<typename AssetType> IndirectAssetReference LoadIndirectAssetReference(const std::string& assetName)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return LoadIndirectAssetReferenceGeneric(AssetType::EnumEntry, assetName);
    }

    IndirectAssetReference LoadIndirectAssetReferenceGeneric(asset_type_t assetType, const std::string& assetName);

    template<typename T> T* GetZoneAssetLoaderState()
    {
        static_assert(std::is_base_of_v<IZoneAssetLoaderState, T>, "T must inherit IZoneAssetLoaderState");
        // T must also have a public default constructor

        const auto foundEntry = m_zone_asset_loader_states.find(typeid(T));
        if (foundEntry != m_zone_asset_loader_states.end())
            return dynamic_cast<T*>(foundEntry->second.get());

        auto newState = std::make_unique<T>();
        newState->SetZone(m_zone);
        auto* newStatePtr = newState.get();
        m_zone_asset_loader_states.emplace(std::make_pair<std::type_index, std::unique_ptr<IZoneAssetLoaderState>>(typeid(T), std::move(newState)));
        return newStatePtr;
    }

private:
    [[nodiscard]] XAssetInfoGeneric* LoadDefaultAssetDependency(asset_type_t assetType, const std::string& assetName);

    Zone* m_zone;
    const AssetCreatorCollection* m_creators;
    const IgnoredAssetLookup* m_ignored_asset_lookup;
    std::unordered_map<std::type_index, std::unique_ptr<IZoneAssetLoaderState>> m_zone_asset_loader_states;
};

#include "AssetCreatorCollection.h"
