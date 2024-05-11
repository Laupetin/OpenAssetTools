#pragma once

#include "AssetLoadingContext.h"
#include "IAssetLoader.h"
#include "IAssetLoadingManager.h"

#include <map>

class AssetLoadingManager final : public IAssetLoadingManager
{
public:
    AssetLoadingManager(const std::map<asset_type_t, std::unique_ptr<IAssetLoader>>& assetLoadersByType, AssetLoadingContext& context);

    bool LoadAssetFromLoader(asset_type_t assetType, const std::string& assetName);

    [[nodiscard]] AssetLoadingContext* GetAssetLoadingContext() const override;

    XAssetInfoGeneric* AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo) override;
    XAssetInfoGeneric* LoadDependency(asset_type_t assetType, const std::string& assetName) override;
    IndirectAssetReference LoadIndirectAssetReference(asset_type_t assetType, const std::string& assetName) override;

private:
    XAssetInfoGeneric* LoadIgnoredDependency(asset_type_t assetType, const std::string& assetName, IAssetLoader* loader);
    XAssetInfoGeneric* LoadAssetDependency(asset_type_t assetType, const std::string& assetName, const IAssetLoader* loader);

    XAssetInfoGeneric* AddAssetInternal(std::unique_ptr<XAssetInfoGeneric> xAssetInfo);

    const std::map<asset_type_t, std::unique_ptr<IAssetLoader>>& m_asset_loaders_by_type;
    AssetLoadingContext& m_context;
    XAssetInfoGeneric* m_last_dependency_loaded;
};
