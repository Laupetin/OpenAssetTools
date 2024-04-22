#pragma once
#include "AssetLoadingContext.h"
#include "Game/IAsset.h"
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

    virtual XAssetInfoGeneric* AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo) = 0;

    XAssetInfoGeneric* AddAsset(const asset_type_t assetType, const std::string& assetName, void* asset)
    {
        return AddAsset(assetType, assetName, asset, std::vector<XAssetInfoGeneric*>(), std::vector<scr_string_t>());
    }

    XAssetInfoGeneric* AddAsset(const asset_type_t assetType, const std::string& assetName, void* asset, std::vector<XAssetInfoGeneric*> dependencies)
    {
        return AddAsset(assetType, assetName, asset, std::move(dependencies), std::vector<scr_string_t>());
    }

    XAssetInfoGeneric* AddAsset(const asset_type_t assetType,
                                const std::string& assetName,
                                void* asset,
                                std::vector<XAssetInfoGeneric*> dependencies,
                                std::vector<scr_string_t> usedScriptStrings)
    {
        return AddAsset(assetType, assetName, asset, std::move(dependencies), std::move(usedScriptStrings), std::vector<IndirectAssetReference>());
    }

    XAssetInfoGeneric* AddAsset(const asset_type_t assetType,
                                const std::string& assetName,
                                void* asset,
                                std::vector<XAssetInfoGeneric*> dependencies,
                                std::vector<scr_string_t> usedScriptStrings,
                                std::vector<IndirectAssetReference> indirectAssetReferences)
    {
        return AddAsset(std::make_unique<XAssetInfoGeneric>(
            assetType, assetName, asset, std::move(dependencies), std::move(usedScriptStrings), std::move(indirectAssetReferences)));
    }

    template<typename AssetType> XAssetInfo<typename AssetType::Type>* LoadDependency(const std::string& assetName)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(LoadDependency(AssetType::EnumEntry, assetName));
    }

    template<typename AssetType> IndirectAssetReference LoadIndirectAssetReference(const std::string& assetName)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return LoadIndirectAssetReference(AssetType::EnumEntry, assetName);
    }

protected:
    virtual XAssetInfoGeneric* LoadDependency(asset_type_t assetType, const std::string& assetName) = 0;
    virtual IndirectAssetReference LoadIndirectAssetReference(asset_type_t assetType, const std::string& assetName) = 0;
};
