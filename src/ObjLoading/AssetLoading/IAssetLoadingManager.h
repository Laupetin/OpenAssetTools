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

    template<typename AssetType> XAssetInfo<typename AssetType::Type>* AddAsset(const std::string& assetName, typename AssetType::Type* asset)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(AddAsset(std::make_unique<XAssetInfoGeneric>(
            AssetType::EnumEntry, assetName, asset, std::vector<XAssetInfoGeneric*>(), std::vector<scr_string_t>(), std::vector<IndirectAssetReference>())));
    }

    template<typename AssetType>
    XAssetInfo<typename AssetType::Type>* AddAsset(const std::string& assetName, typename AssetType::Type* asset, std::vector<XAssetInfoGeneric*> dependencies)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(AddAsset(std::make_unique<XAssetInfoGeneric>(
            AssetType::EnumEntry, assetName, asset, std::move(dependencies), std::vector<scr_string_t>(), std::vector<IndirectAssetReference>())));
    }

    template<typename AssetType>
    XAssetInfo<typename AssetType::Type>* AddAsset(const std::string& assetName,
                                                   typename AssetType::Type* asset,
                                                   std::vector<XAssetInfoGeneric*> dependencies,
                                                   std::vector<scr_string_t> usedScriptStrings)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(AddAsset(std::make_unique<XAssetInfoGeneric>(
            AssetType::EnumEntry, assetName, asset, std::move(dependencies), std::move(usedScriptStrings), std::vector<IndirectAssetReference>())));
    }

    template<typename AssetType>
    XAssetInfo<typename AssetType::Type>* AddAsset(const std::string& assetName,
                                                   typename AssetType::Type* asset,
                                                   std::vector<XAssetInfoGeneric*> dependencies,
                                                   std::vector<scr_string_t> usedScriptStrings,
                                                   std::vector<IndirectAssetReference> indirectAssetReferences)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(AddAsset(std::make_unique<XAssetInfoGeneric>(
            AssetType::EnumEntry, assetName, asset, std::move(dependencies), std::move(usedScriptStrings), std::move(indirectAssetReferences))));
    }

    template<typename AssetType> XAssetInfo<typename AssetType::Type>* AddAsset(std::unique_ptr<XAssetInfo<typename AssetType::Type>> xAssetInfo)
    {
        static_assert(std::is_base_of_v<IAssetBase, AssetType>);

        return static_cast<XAssetInfo<typename AssetType::Type>*>(AddAsset(std::unique_ptr<XAssetInfoGeneric>(xAssetInfo.release())));
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
    virtual XAssetInfoGeneric* AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo) = 0;
    virtual XAssetInfoGeneric* LoadDependency(asset_type_t assetType, const std::string& assetName) = 0;
    virtual IndirectAssetReference LoadIndirectAssetReference(asset_type_t assetType, const std::string& assetName) = 0;
};
