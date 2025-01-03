#pragma once
#include "Asset/IAssetCreator.h"
#include "Pool/GlobalAssetPool.h"

template<typename AssetType> class GlobalAssetPoolsLoader : public AssetCreator<AssetType>
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    GlobalAssetPoolsLoader(Zone& zone)
        : m_zone(zone)
    {
    }

    AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
    {
        auto* existingAsset = GlobalAssetPool<typename AssetType::Type>::GetAssetByName(assetName);

        if (!existingAsset)
            return AssetCreationResult::NoAction();

        AssetRegistration<AssetType> registration(assetName, existingAsset->Asset());

        for (const auto* dependency : existingAsset->m_dependencies)
        {
            auto* newDependency = context.LoadDependencyGeneric(dependency->m_type, dependency->m_name);
            if (newDependency)
                registration.AddDependency(newDependency);
            else
                return AssetCreationResult::Failure();
        }

        for (const auto& indirectAssetReference : existingAsset->m_indirect_asset_references)
            registration.AddIndirectAssetReference(context.LoadIndirectAssetReferenceGeneric(indirectAssetReference.m_type, indirectAssetReference.m_name));

        // Make sure any used script string is available in the created zone
        // The replacement of the scr_string_t values will be done upon writing
        for (const auto scrString : existingAsset->m_used_script_strings)
            m_zone.m_script_strings.AddOrGetScriptString(existingAsset->m_zone->m_script_strings.CValue(scrString));

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }

private:
    Zone& m_zone;
};
