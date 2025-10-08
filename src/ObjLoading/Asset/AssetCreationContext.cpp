#include "AssetCreationContext.h"

#include <cassert>
#include <format>
#include <iostream>

IgnoredAssetLookup::IgnoredAssetLookup() = default;

IgnoredAssetLookup::IgnoredAssetLookup(const AssetList& assetList)
{
    m_ignored_asset_lookup.reserve(assetList.m_entries.size());
    for (const auto& asset : assetList.m_entries)
    {
        m_ignored_asset_lookup.emplace(asset.m_name, asset.m_type);
    }
}

bool IgnoredAssetLookup::IsAssetIgnored(asset_type_t assetType, const std::string& name) const
{
    const auto entries = m_ignored_asset_lookup.equal_range(name);

    for (auto i = entries.first; i != entries.second; ++i)
    {
        if (i->second == assetType)
            return true;
    }

    return false;
}

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
    assert(m_asset);

    std::vector<XAssetInfoGeneric*> dependencies(m_dependencies.begin(), m_dependencies.end());
    std::vector<scr_string_t> scriptStrings(m_used_script_strings.begin(), m_used_script_strings.end());
    std::vector<IndirectAssetReference> indirectAssetReferences(m_indirect_asset_references.begin(), m_indirect_asset_references.end());

    return std::make_unique<XAssetInfoGeneric>(
        m_type, std::move(m_name), m_asset, std::move(dependencies), std::move(scriptStrings), std::move(indirectAssetReferences));
}

AssetCreationContext::AssetCreationContext(Zone& zone, const AssetCreatorCollection* creators, const IgnoredAssetLookup* ignoredAssetLookup)
    : ZoneAssetCreationStateContainer(zone),
      m_zone(zone),
      m_forced_asset_pools(ZoneAssetPools::CreateForGame(zone.m_game->GetId(), &zone, zone.m_priority)),
      m_creators(creators),
      m_ignored_asset_lookup(ignoredAssetLookup),
      m_forced_load_depth(0u)
{
}

XAssetInfoGeneric* AssetCreationContext::AddAssetGeneric(GenericAssetRegistration registration) const
{
    auto xAssetInfo = registration.CreateXAssetInfo();
    xAssetInfo->m_zone = &m_zone;

    const auto assetType = xAssetInfo->m_type;
    const auto* pAssetName = xAssetInfo->m_name.c_str();

    XAssetInfoGeneric* addedAsset;
    if (m_forced_load_depth > 0)
        addedAsset = m_forced_asset_pools->AddAsset(std::move(xAssetInfo));
    else
        addedAsset = m_zone.m_pools->AddAsset(std::move(xAssetInfo));

    if (addedAsset == nullptr)
        std::cerr << std::format("Failed to add asset of type \"{}\" to pool: \"{}\"\n", *m_zone.m_pools->GetAssetTypeName(assetType), pAssetName);
    return addedAsset;
}

XAssetInfoGeneric* AssetCreationContext::LoadDefaultAssetDependency(const asset_type_t assetType, const std::string& assetName)
{
    const auto result = m_creators->CreateDefaultAsset(assetType, assetName, *this);
    if (result.HasTakenAction() && !result.HasFailed())
        return result.GetAssetInfo();

    std::cerr << std::format("Failed to create default asset of type {}\n", *m_zone.m_pools->GetAssetTypeName(assetType));

    return nullptr;
}

XAssetInfoGeneric* AssetCreationContext::LoadDependencyGeneric(const asset_type_t assetType, const std::string& assetName)
{
    auto* alreadyLoadedAsset = m_zone.m_pools->GetAssetOrAssetReference(assetType, assetName);
    if (alreadyLoadedAsset)
        return alreadyLoadedAsset;

    if (m_forced_load_depth > 0)
    {
        alreadyLoadedAsset = m_forced_asset_pools->GetAssetOrAssetReference(assetType, assetName);
        if (alreadyLoadedAsset)
            return alreadyLoadedAsset;

        // If we are already force loading an asset we should not load its dependencies
        return LoadDefaultAssetDependency(assetType, std::format(",{}", assetName));
    }

    if (m_ignored_asset_lookup->IsAssetIgnored(assetType, assetName))
        return LoadDefaultAssetDependency(assetType, std::format(",{}", assetName));

    const auto result = m_creators->CreateAsset(assetType, assetName, *this);
    if (result.HasTakenAction())
    {
        if (!result.HasFailed())
            return result.GetAssetInfo();

        std::cerr << std::format("Could not load asset \"{}\" of type \"{}\"\n", assetName, *m_zone.m_pools->GetAssetTypeName(assetType));
    }
    else
    {
        std::cerr << std::format("Missing asset \"{}\" of type \"{}\"\n", assetName, *m_zone.m_pools->GetAssetTypeName(assetType));
    }

    return nullptr;
}

IndirectAssetReference AssetCreationContext::LoadIndirectAssetReferenceGeneric(const asset_type_t assetType, const std::string& assetName)
{
    const auto* alreadyLoadedAsset = m_zone.m_pools->GetAssetOrAssetReference(assetType, assetName);
    if (alreadyLoadedAsset)
        return IndirectAssetReference(assetType, assetName);

    if (m_ignored_asset_lookup->IsAssetIgnored(assetType, assetName))
        return IndirectAssetReference(assetType, assetName);

    const auto result = m_creators->CreateAsset(assetType, assetName, *this);
    if (!result.HasTakenAction() && !result.HasFailed())
    {
        std::cerr << std::format("Could not load indirectly referenced asset \"{}\" of type \"{}\"\n", assetName, *m_zone.m_pools->GetAssetTypeName(assetType));
    }
    return IndirectAssetReference(assetType, assetName);
}

XAssetInfoGeneric* AssetCreationContext::ForceLoadDependencyGeneric(const asset_type_t assetType, const std::string& assetName)
{
    auto* alreadyLoadedAsset = m_zone.m_pools->GetAssetOrAssetReference(assetType, assetName);
    if (alreadyLoadedAsset && !alreadyLoadedAsset->IsReference())
        return alreadyLoadedAsset;
    alreadyLoadedAsset = m_forced_asset_pools->GetAssetOrAssetReference(assetType, assetName);
    if (alreadyLoadedAsset && !alreadyLoadedAsset->IsReference())
        return alreadyLoadedAsset;

    auto result = AssetCreationResult::NoAction();
    if (m_ignored_asset_lookup->IsAssetIgnored(assetType, assetName))
    {
        // Load default asset to zone
        if (!LoadDefaultAssetDependency(assetType, std::format(",{}", assetName)))
            return nullptr;

        ++m_forced_load_depth;

        result = m_creators->CreateAsset(assetType, assetName, *this);

        assert(m_forced_load_depth > 0);
        m_forced_load_depth = std::min(m_forced_load_depth - 1u, 0u);
    }
    else
        result = m_creators->CreateAsset(assetType, assetName, *this);

    if (result.HasTakenAction())
    {
        if (!result.HasFailed())
            return result.GetAssetInfo();

        std::cerr << std::format("Could not load asset \"{}\" of type \"{}\"\n", assetName, *m_zone.m_pools->GetAssetTypeName(assetType));
    }
    else
    {
        std::cerr << std::format("Missing asset \"{}\" of type \"{}\"\n", assetName, *m_zone.m_pools->GetAssetTypeName(assetType));
    }

    return nullptr;
}
