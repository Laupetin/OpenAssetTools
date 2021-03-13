#pragma once
#include <unordered_map>

#include "AssetLoadingContext.h"
#include "IAssetLoader.h"
#include "IAssetLoadingManager.h"

class AssetLoadingManager final : public IAssetLoadingManager
{
    const std::unordered_map<asset_type_t, std::unique_ptr<IAssetLoader>>& m_asset_loaders_by_type;
    AssetLoadingContext& m_context;
    XAssetInfoGeneric* m_last_dependency_loaded;

    XAssetInfoGeneric* LoadIgnoredDependency(asset_type_t assetType, const std::string& assetName, IAssetLoader* loader);
    XAssetInfoGeneric* LoadAssetDependency(asset_type_t assetType, const std::string& assetName, IAssetLoader* loader);

public:
    AssetLoadingManager(const std::unordered_map<asset_type_t, std::unique_ptr<IAssetLoader>>& assetLoadersByType, AssetLoadingContext& context);

    bool LoadAssetFromLoader(asset_type_t assetType, const std::string& assetName);
    void AddAsset(asset_type_t assetType, const std::string& assetName, void* asset, std::vector<XAssetInfoGeneric*>& dependencies) override;
    XAssetInfoGeneric* LoadDependency(asset_type_t assetType, const std::string& assetName) override;
};
