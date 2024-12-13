#include "AssetCreationContext.h"

#include <iostream>

GenericAssetRegistration::GenericAssetRegistration(const asset_type_t type, std::string name, void* asset)
    : m_type(type),
      m_name(std::move(name)),
      m_asset(asset)
{
}

void GenericAssetRegistration::AddDependency(XAssetInfoGeneric* dependency)
{
    m_dependencies.emplace(dependency);
}

void GenericAssetRegistration::AddScriptString(scr_string_t scriptString)
{
    m_used_script_strings.emplace(scriptString);
}

void GenericAssetRegistration::AddIndirectAssetReference(IndirectAssetReference indirectAssetReference)
{
    m_indirect_asset_references.emplace(std::move(indirectAssetReference));
}

std::unique_ptr<XAssetInfoGeneric> GenericAssetRegistration::CreateXAssetInfo()
{
    std::vector<XAssetInfoGeneric*> dependencies(m_dependencies.begin(), m_dependencies.end());
    std::vector<scr_string_t> scriptStrings(m_used_script_strings.begin(), m_used_script_strings.end());
    std::vector<IndirectAssetReference> indirectAssetReferences(m_indirect_asset_references.begin(), m_indirect_asset_references.end());

    return std::make_unique<XAssetInfoGeneric>(
        m_type, std::move(m_name), m_asset, std::move(dependencies), std::move(scriptStrings), std::move(indirectAssetReferences));
}

AssetCreationContext::AssetCreationContext(Zone* zone, const AssetCreatorCollection* creators, const IgnoredAssetLookup* ignoredAssetLookup)
    : m_zone(zone),
      m_creators(creators),
      m_ignored_asset_lookup(ignoredAssetLookup)
{
}

XAssetInfoGeneric* AssetCreationContext::AddAssetGeneric(GenericAssetRegistration& registration) const
{
    auto xAssetInfo = registration.CreateXAssetInfo();
    xAssetInfo->m_zone = m_zone;

    const auto assetType = xAssetInfo->m_type;
    const auto* pAssetName = xAssetInfo->m_name.c_str();

    auto* addedAsset = m_zone->m_pools->AddAsset(std::move(xAssetInfo));
    if (addedAsset == nullptr)
        std::cerr << std::format("Failed to add asset of type \"{}\" to pool: \"{}\"\n", m_zone->m_pools->GetAssetTypeName(assetType), pAssetName);

    return addedAsset;
}

XAssetInfoGeneric* AssetCreationContext::LoadDefaultAssetDependency(const asset_type_t assetType, const std::string& assetName) const
{
    const auto result = m_creators->CreateDefaultAsset(assetType, assetName, *m_zone->GetMemory(), *this);
    if (result.HasTakenAction() && !result.HasFailed())
        return result.GetAssetInfo();

    std::cerr << "";

    return nullptr;
}

XAssetInfoGeneric* AssetCreationContext::LoadDependencyGeneric(const asset_type_t assetType, const std::string& assetName)
{
    auto* alreadyLoadedAsset = m_zone->m_pools->GetAssetOrAssetReference(assetType, assetName);
    if (alreadyLoadedAsset)
        return alreadyLoadedAsset;

    if (m_ignored_asset_lookup->IsAssetIgnored(assetType, assetName))
        return LoadDefaultAssetDependency(assetType, std::format(",{}", assetName));

    m_creators->CreateAsset(assetType, assetName, m_search_path, *m_zone->GetMemory(), *this);
}

IndirectAssetReference AssetCreationContext::LoadIndirectAssetReferenceGeneric(asset_type_t assetType, const std::string& assetName) {}
