#pragma once

#include "AssetCreationContext.h"
#include "Game/IGame.h"
#include "IAssetCreator.h"
#include "IAssetPostProcessor.h"
#include "IDefaultAssetCreator.h"
#include "Zone/ZoneTypes.h"

#include <memory>
#include <vector>

class AssetCreationContext;
class IAssetCreator;
class IAssetPostProcessor;
class AssetCreationResult;
class IDefaultAssetCreator;

class AssetCreatorCollection
{
public:
    explicit AssetCreatorCollection(const Zone& zone);

    void AddAssetCreator(std::unique_ptr<IAssetCreator> creator);
    void AddAssetPostProcessor(std::unique_ptr<IAssetPostProcessor> postProcessor);
    void AddDefaultAssetCreator(std::unique_ptr<IDefaultAssetCreator> defaultAssetCreator);

    AssetCreationResult CreateAsset(asset_type_t assetType, const std::string& assetName, AssetCreationContext& context) const;
    AssetCreationResult CreateDefaultAsset(asset_type_t assetType, const std::string& assetName, AssetCreationContext& context) const;
    void FinalizeZone(AssetCreationContext& context) const;

private:
    std::vector<std::vector<IAssetCreator*>> m_asset_creators_by_type;
    std::vector<std::unique_ptr<IAssetCreator>> m_asset_creators;
    std::vector<std::vector<IAssetPostProcessor*>> m_asset_post_processors_by_type;
    std::vector<std::unique_ptr<IAssetPostProcessor>> m_asset_post_processors;
    std::vector<std::unique_ptr<IDefaultAssetCreator>> m_default_asset_creators_by_type;
};
