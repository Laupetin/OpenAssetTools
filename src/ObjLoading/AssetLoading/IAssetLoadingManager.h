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

    virtual void AddAsset(asset_type_t assetType, const std::string& assetName, void* asset, std::vector<XAssetInfoGeneric*> dependencies, std::vector<scr_string_t> usedScriptStrings) = 0;
    void AddAsset(const asset_type_t assetType, const std::string& assetName, void* asset)
    {
        AddAsset(assetType, assetName, asset, std::vector<XAssetInfoGeneric*>(), std::vector<scr_string_t>());
    }
    virtual XAssetInfoGeneric* LoadDependency(asset_type_t assetType, const std::string& assetName) = 0;
};
