#pragma once

#include "Game/IAsset.h"
#include "Pool/XAssetInfo.h"
#include "Zone/ZoneTypes.h"

#include <memory>
#include <string>
#include <unordered_set>

class GenericAssetRegistration
{
protected:
    GenericAssetRegistration(asset_type_t type, std::string name, void* asset);

public:
    GenericAssetRegistration(const GenericAssetRegistration& other) = delete;
    GenericAssetRegistration(GenericAssetRegistration&& other) = default;
    GenericAssetRegistration& operator=(const GenericAssetRegistration& other) = delete;
    GenericAssetRegistration& operator=(GenericAssetRegistration&& other) noexcept = default;

    void AddDependency(XAssetInfoGeneric* dependency);
    void AddScriptString(scr_string_t scriptString);
    void AddIndirectAssetReference(IndirectAssetReference indirectAssetReference);

    std::unique_ptr<XAssetInfoGeneric> CreateXAssetInfo();

protected:
    asset_type_t m_type;
    std::string m_name;
    void* m_asset;
    std::unordered_set<XAssetInfoGeneric*> m_dependencies;
    std::unordered_set<scr_string_t> m_used_script_strings;
    std::unordered_set<IndirectAssetReference> m_indirect_asset_references;
};

template<AssetOrSubAssetDefinition Asset_t> class AssetRegistration : public GenericAssetRegistration
{
public:
    explicit AssetRegistration(std::string assetName)
        : GenericAssetRegistration(Asset_t::EnumEntry, std::move(assetName), nullptr)
    {
    }

    AssetRegistration(std::string assetName, typename Asset_t::Type* asset)
        : GenericAssetRegistration(Asset_t::EnumEntry, std::move(assetName), asset)
    {
    }

    AssetRegistration(const AssetRegistration& other) = delete;
    AssetRegistration(AssetRegistration&& other) = default;
    AssetRegistration& operator=(const AssetRegistration& other) = delete;
    AssetRegistration& operator=(AssetRegistration&& other) noexcept = default;

    void SetAsset(typename Asset_t::Type* asset)
    {
        m_asset = asset;
    }
};
