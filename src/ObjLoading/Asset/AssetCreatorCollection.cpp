#include "AssetCreatorCollection.h"

#include <cassert>

AssetCreatorCollection::AssetCreatorCollection(const Zone& zone)
{
    m_asset_creators_by_type.resize(zone.m_pools->GetAssetTypeCount());
    m_asset_post_processors_by_type.resize(zone.m_pools->GetAssetTypeCount());
    m_default_asset_creators_by_type.resize(zone.m_pools->GetAssetTypeCount());
}

void AssetCreatorCollection::AddAssetCreator(std::unique_ptr<IAssetCreator> creator)
{
    const auto maybeHandlingAssetType = creator->GetHandlingAssetType();
    assert(!maybeHandlingAssetType || static_cast<unsigned>(*maybeHandlingAssetType) < m_asset_creators_by_type.size());
    if (maybeHandlingAssetType && static_cast<unsigned>(*maybeHandlingAssetType) < m_asset_creators_by_type.size())
        m_asset_creators_by_type[static_cast<unsigned>(*maybeHandlingAssetType)].emplace_back(creator.get());

    m_asset_creators.emplace_back(std::move(creator));
}

void AssetCreatorCollection::AddAssetPostProcessor(std::unique_ptr<IAssetPostProcessor> postProcessor)
{
    const auto handlingAssetType = postProcessor->GetHandlingAssetType();
    assert(static_cast<unsigned>(handlingAssetType) < m_asset_post_processors_by_type.size());
    if (static_cast<unsigned>(handlingAssetType) < m_asset_post_processors_by_type.size())
        m_asset_post_processors_by_type[static_cast<unsigned>(handlingAssetType)].emplace_back(postProcessor.get());

    m_asset_post_processors.emplace_back(std::move(postProcessor));
}

void AssetCreatorCollection::AddDefaultAssetCreator(std::unique_ptr<IDefaultAssetCreator> defaultAssetCreator)
{
    const auto handlingAssetType = defaultAssetCreator->GetHandlingAssetType();
    assert(static_cast<unsigned>(handlingAssetType) < m_default_asset_creators_by_type.size());
    assert(!m_default_asset_creators_by_type[handlingAssetType]);

    if (static_cast<unsigned>(handlingAssetType) < m_default_asset_creators_by_type.size())
        m_default_asset_creators_by_type[handlingAssetType] = std::move(defaultAssetCreator);
}

AssetCreationResult AssetCreatorCollection::CreateAsset(const asset_type_t assetType, const std::string& assetName, AssetCreationContext& context) const
{
    assert(assetType >= 0 && static_cast<unsigned>(assetType) < m_asset_creators_by_type.size());

    if (assetType >= 0 && static_cast<unsigned>(assetType) < m_asset_creators_by_type.size())
    {
        for (const auto& creator : m_asset_creators_by_type[assetType])
        {
            const auto result = creator->CreateAsset(assetName, context);
            if (result.HasTakenAction())
            {
                // Post process asset if creation was successful
                if (result.HasBeenSuccessful())
                {
                    assert(static_cast<unsigned>(assetType) < m_asset_post_processors_by_type.size());
                    for (auto* postProcessor : m_asset_post_processors_by_type[assetType])
                        postProcessor->PostProcessAsset(*result.GetAssetInfo(), context);
                }

                // Return result that was either successful or failed
                return result;
            }
        }
    }

    return AssetCreationResult::NoAction();
}

AssetCreationResult AssetCreatorCollection::CreateDefaultAsset(const asset_type_t assetType, const std::string& assetName, AssetCreationContext& context) const
{
    assert(assetType >= 0 && static_cast<unsigned>(assetType) < m_default_asset_creators_by_type.size());

    if (assetType >= 0 && static_cast<unsigned>(assetType) < m_default_asset_creators_by_type.size() && m_default_asset_creators_by_type[assetType])
    {
        return m_default_asset_creators_by_type[assetType]->CreateDefaultAsset(assetName, context);
    }

    return AssetCreationResult::NoAction();
}

void AssetCreatorCollection::FinalizeZone(AssetCreationContext& context) const
{
    for (const auto& creator : m_asset_creators)
        creator->FinalizeZone(context);
    for (const auto& postProcessor : m_asset_post_processors)
        postProcessor->FinalizeZone(context);
}
