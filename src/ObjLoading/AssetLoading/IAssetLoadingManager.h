#pragma once
#include "AssetLoadingContext.h"
#include "Pool/XAssetInfo.h"
#include "Utils/ClassUtils.h"
#include "Zone/ZoneTypes.h"

#include <string>

class IAssetLoadingManager
{
public:
    IAssetLoadingManager() = default;
    virtual ~IAssetLoadingManager() = default;
    IAssetLoadingManager(const IAssetLoadingManager& other) = default;
    IAssetLoadingManager(IAssetLoadingManager&& other) noexcept = default;
    IAssetLoadingManager& operator=(const IAssetLoadingManager& other) = default;
    IAssetLoadingManager& operator=(IAssetLoadingManager&& other) noexcept = default;

    _NODISCARD virtual AssetLoadingContext* GetAssetLoadingContext() const = 0;

    virtual XAssetInfoGeneric* AddAsset(asset_type_t assetType,
                                        const std::string& assetName,
                                        void* asset,
                                        std::vector<XAssetInfoGeneric*> dependencies,
                                        std::vector<scr_string_t> usedScriptStrings) = 0;

    XAssetInfoGeneric* AddAsset(const asset_type_t assetType, const std::string& assetName, void* asset)
    {
        return AddAsset(assetType, assetName, asset, std::vector<XAssetInfoGeneric*>(), std::vector<scr_string_t>());
    }

    virtual XAssetInfoGeneric* LoadDependency(asset_type_t assetType, const std::string& assetName) = 0;
};
