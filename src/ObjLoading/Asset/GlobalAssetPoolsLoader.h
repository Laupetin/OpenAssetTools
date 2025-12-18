#pragma once

#include "Asset/IAssetCreator.h"
#include "Marking/AssetVisitor.h"
#include "Marking/BaseAssetMarker.h"
#include "Pool/GlobalAssetPool.h"

class GlobalAssetPoolsAssetStealer : public AssetVisitor
{
public:
    GlobalAssetPoolsAssetStealer(GenericAssetRegistration& registration, Zone& zone, Zone& foreignZone, AssetCreationContext& context);

    std::optional<XAssetInfoGeneric*> Visit_Dependency(asset_type_t assetType, const char* assetName) override;
    std::optional<scr_string_t> Visit_ScriptString(scr_string_t scriptString) override;
    void Visit_IndirectAssetRef(asset_type_t assetType, const char* assetName) override;

    [[nodiscard]] bool Failed() const;

private:
    GenericAssetRegistration& m_registration;
    Zone& m_zone;
    Zone& m_foreign_zone;
    AssetCreationContext& m_context;

    bool m_failure;
};

template<typename AssetType> class GlobalAssetPoolsLoader : public AssetCreator<AssetType>
{
public:
    static_assert(std::is_base_of_v<IAssetBase, AssetType>);

    explicit GlobalAssetPoolsLoader(Zone& zone)
        : m_zone(zone)
    {
    }

    AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
    {
        auto* existingAsset = GlobalAssetPool<typename AssetType::Type>::GetAssetByName(assetName);

        if (!existingAsset)
            return AssetCreationResult::NoAction();

        AssetRegistration<AssetType> registration(assetName, existingAsset->Asset());

        GlobalAssetPoolsAssetStealer stealer(registration, m_zone, *existingAsset->m_zone, context);
        AssetMarker<AssetType> marker(stealer);
        marker.Mark(existingAsset->Asset());

        auto* newAsset = context.AddAsset(std::move(registration));

        return AssetCreationResult::Success(newAsset);
    }

private:
    Zone& m_zone;
};
