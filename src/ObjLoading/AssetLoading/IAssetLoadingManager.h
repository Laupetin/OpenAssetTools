#pragma once
#include <string>

#include "Pool/XAssetInfo.h"
#include "Zone/ZoneTypes.h"

class IAssetLoadingManager
{
public:
    IAssetLoadingManager() = default;
    virtual ~IAssetLoadingManager() = default;
    IAssetLoadingManager(const IAssetLoadingManager& other) = default;
    IAssetLoadingManager(IAssetLoadingManager&& other) noexcept = default;
    IAssetLoadingManager& operator=(const IAssetLoadingManager& other) = default;
    IAssetLoadingManager& operator=(IAssetLoadingManager&& other) noexcept = default;

    virtual void AddAsset(asset_type_t assetType, const std::string& assetName, void* asset, std::vector<XAssetInfoGeneric*>& dependencies) = 0;
    void AddAsset(const asset_type_t assetType, const std::string& assetName, void* asset)
    {
        std::vector<XAssetInfoGeneric*> dependencies;
        AddAsset(assetType, assetName, asset, dependencies);
    }
    virtual XAssetInfoGeneric* LoadDependency(asset_type_t assetType, const std::string& assetName) = 0;
};
