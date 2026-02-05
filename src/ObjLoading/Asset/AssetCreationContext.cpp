#include "AssetCreationContext.h"

#include "Utils/Logging/Log.h"

#include <cassert>
#include <format>

IgnoredAssetLookup::IgnoredAssetLookup() = default;

IgnoredAssetLookup::IgnoredAssetLookup(const AssetList& assetList)
{
    m_ignored_asset_lookup.reserve(assetList.m_entries.size());
    for (const auto& asset : assetList.m_entries)
    {
        m_ignored_asset_lookup.emplace(asset.m_name, asset.m_type);
    }
}

bool IgnoredAssetLookup::IsAssetIgnored(const asset_type_t assetType, const std::string& name) const
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
      m_forced_asset_pools(std::make_unique<ZoneAssetPools>(zone, zone.m_priority)),
      m_sub_asset_pools(IGame::GetGameById(zone.m_game_id)->GetSubAssetTypeCount()),
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
        addedAsset = m_zone.m_pools.AddAsset(std::move(xAssetInfo));

    if (addedAsset == nullptr)
        con::error(R"(Failed to add asset of type "{}" to pool: "{}")", *IGame::GetGameById(m_zone.m_game_id)->GetAssetTypeName(assetType), pAssetName);

    return addedAsset;
}

XAssetInfoGeneric* AssetCreationContext::AddSubAssetGeneric(GenericAssetRegistration registration) const
{
    auto xAssetInfo = registration.CreateXAssetInfo();
    xAssetInfo->m_zone = &m_zone;

    const auto subAssetType = xAssetInfo->m_type;
    const auto* pAssetName = xAssetInfo->m_name.c_str();

    auto* addedAsset = m_sub_asset_pools[subAssetType]->AddAsset(std::move(xAssetInfo));

    if (addedAsset == nullptr)
        con::error(R"(Failed to add sub asset of type "{}" to pool: "{}")", *IGame::GetGameById(m_zone.m_game_id)->GetAssetTypeName(subAssetType), pAssetName);

    return addedAsset;
}

XAssetInfoGeneric* AssetCreationContext::LoadDefaultAssetDependency(const asset_type_t assetType, const std::string& assetName)
{
    const auto result = m_creators->CreateDefaultAsset(assetType, assetName, *this);
    if (result.HasTakenAction() && !result.HasFailed())
        return result.GetAssetInfo();

    con::error("Failed to create default asset of type {}", *IGame::GetGameById(m_zone.m_game_id)->GetAssetTypeName(assetType));

    return nullptr;
}

XAssetInfoGeneric* AssetCreationContext::LoadDependencyGeneric(const asset_type_t assetType, const std::string& assetName, const bool required)
{
    auto* alreadyLoadedAsset = m_zone.m_pools.GetAssetOrAssetReference(assetType, assetName);
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

        con::error(R"(Could not load asset "{}" of type "{}")", assetName, *IGame::GetGameById(m_zone.m_game_id)->GetAssetTypeName(assetType));
    }
    else if (required)
    {
        con::error(R"(Missing asset "{}" of type "{}")", assetName, *IGame::GetGameById(m_zone.m_game_id)->GetAssetTypeName(assetType));
    }

    return nullptr;
}

XAssetInfoGeneric* AssetCreationContext::LoadSubAssetGeneric(const asset_type_t subAssetType, const std::string& assetName)
{
    assert(subAssetType < m_sub_asset_pools.size());
    auto* alreadyLoadedSubAsset = m_sub_asset_pools[subAssetType]->GetAsset(assetName);
    if (alreadyLoadedSubAsset)
        return alreadyLoadedSubAsset;

    const auto result = m_creators->CreateSubAsset(subAssetType, assetName, *this);
    if (result.HasTakenAction())
    {
        if (!result.HasFailed())
            return result.GetAssetInfo();

        con::error(R"(Could not load sub asset "{}" of type "{}")", assetName, *IGame::GetGameById(m_zone.m_game_id)->GetSubAssetTypeName(subAssetType));
    }

    return nullptr;
}

IndirectAssetReference AssetCreationContext::LoadIndirectAssetReferenceGeneric(const asset_type_t assetType, const std::string& assetName)
{
    const auto* alreadyLoadedAsset = m_zone.m_pools.GetAssetOrAssetReference(assetType, assetName);
    if (alreadyLoadedAsset)
        return IndirectAssetReference(assetType, assetName);

    if (m_ignored_asset_lookup->IsAssetIgnored(assetType, assetName))
        return IndirectAssetReference(assetType, assetName);

    const auto result = m_creators->CreateAsset(assetType, assetName, *this);
    if (!result.HasTakenAction() && !result.HasFailed())
    {
        con::warn(
            R"(Could not load indirectly referenced asset "{}" of type "{}")", assetName, *IGame::GetGameById(m_zone.m_game_id)->GetAssetTypeName(assetType));
    }
    return IndirectAssetReference(assetType, assetName);
}

XAssetInfoGeneric* AssetCreationContext::ForceLoadDependencyGeneric(const asset_type_t assetType, const std::string& assetName)
{
    auto* alreadyLoadedAsset = m_zone.m_pools.GetAssetOrAssetReference(assetType, assetName);
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

        con::error(R"(Could not load asset "{}" of type "{}")", assetName, *IGame::GetGameById(m_zone.m_game_id)->GetAssetTypeName(assetType));
    }
    else
    {
        con::error(R"(Missing asset "{}" of type "{}")", assetName, *IGame::GetGameById(m_zone.m_game_id)->GetAssetTypeName(assetType));
    }

    return nullptr;
}
