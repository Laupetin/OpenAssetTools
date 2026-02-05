#pragma once

#include "Asset/IAssetCreator.h"
#include "Marking/AssetVisitor.h"
#include "Marking/BaseAssetMarker.h"
#include "Pool/GlobalAssetPool.h"

class GlobalAssetPoolsRegistrationPreparation : public AssetVisitor
{
public:
    GlobalAssetPoolsRegistrationPreparation(GenericAssetRegistration& registration, Zone& zone, Zone& foreignZone, AssetCreationContext& context);

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

template<AssetDefinition Asset_t> class GlobalAssetPoolsLoader : public AssetCreator<Asset_t>
{
public:
    explicit GlobalAssetPoolsLoader(Zone& zone)
        : m_zone(zone)
    {
    }

    AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
    {
        auto* existingAsset = GameGlobalAssetPools::GetGlobalPoolsForGame(m_zone.m_game_id)->GetAsset<Asset_t>(assetName);

        if (!existingAsset)
            return AssetCreationResult::NoAction();

        AssetRegistration<Asset_t> registration(assetName, existingAsset->Asset());

        GlobalAssetPoolsRegistrationPreparation registrationPreparation(registration, m_zone, *existingAsset->m_zone, context);
        AssetMarker<Asset_t> marker(registrationPreparation);
        marker.Mark(existingAsset->Asset());

        auto* newAsset = context.AddAsset(std::move(registration));
        // Make sure we remember this asset came from a different zone
        newAsset->m_zone = existingAsset->m_zone;

        return AssetCreationResult::Success(newAsset);
    }

private:
    Zone& m_zone;
};
