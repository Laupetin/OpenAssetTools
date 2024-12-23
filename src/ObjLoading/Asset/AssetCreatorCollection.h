#pragma once

#include "AssetCreationContext.h"
#include "Game/IGame.h"
#include "IAssetCreator.h"
#include "IDefaultAssetCreator.h"
#include "Zone/ZoneTypes.h"

#include <memory>

class AssetCreationContext;
class IAssetCreator;
class AssetCreationResult;
class IDefaultAssetCreator;

class AssetCreatorCollection
{
public:
    explicit AssetCreatorCollection(const Zone& zone);

    void AddAssetCreator(std::unique_ptr<IAssetCreator> creator);
    void AddDefaultAssetCreator(std::unique_ptr<IDefaultAssetCreator> defaultAssetCreator);

    AssetCreationResult CreateAsset(asset_type_t assetType, const std::string& assetName, AssetCreationContext& context) const;
    AssetCreationResult CreateDefaultAsset(asset_type_t assetType, const std::string& assetName, AssetCreationContext& context) const;
    void FinalizeZone(AssetCreationContext& context) const;

private:
    std::vector<std::vector<IAssetCreator*>> m_asset_creators_by_type;
    std::vector<std::unique_ptr<IDefaultAssetCreator>> m_default_asset_creators_by_type;
    std::vector<std::unique_ptr<IAssetCreator>> m_asset_creators;
};
