#include "AssetInfoCollector.h"

#include <algorithm>
#include <cassert>

AssetInfoCollector::AssetInfoCollector(Zone& zone)
    : m_zone(zone)
{
}

std::vector<XAssetInfoGeneric*> AssetInfoCollector::GetDependencies() const
{
    std::vector<XAssetInfoGeneric*> dependencies;
    if (!m_dependencies.empty())
    {
        dependencies.reserve(m_dependencies.size());
        for (auto dependency : m_dependencies)
            dependencies.push_back(dependency);
    }

    return dependencies;
}

std::vector<scr_string_t> AssetInfoCollector::GetUsedScriptStrings() const
{
    std::vector<scr_string_t> usedScriptStrings;
    if (!m_used_script_strings.empty())
    {
        usedScriptStrings.reserve(m_used_script_strings.size());
        for (auto scrString : m_used_script_strings)
            usedScriptStrings.push_back(scrString);

        std::ranges::sort(usedScriptStrings);
    }

    return usedScriptStrings;
}

std::vector<IndirectAssetReference> AssetInfoCollector::GetIndirectAssetReferences() const
{
    std::vector<IndirectAssetReference> assetReferences;
    if (!m_indirect_asset_references.empty())
    {
        assetReferences.reserve(m_indirect_asset_references.size());
        for (const auto& assetReference : m_indirect_asset_references)
            assetReferences.emplace_back(assetReference);
    }

    return assetReferences;
}

std::optional<XAssetInfoGeneric*> AssetInfoCollector::Visit_Dependency(const asset_type_t assetType, const char* assetName)
{
    auto* assetInfo = m_zone.m_pools.GetAsset(assetType, assetName);
    if (assetInfo == nullptr)
        return std::nullopt;

    const auto existingEntry = m_dependencies.find(assetInfo);
    if (existingEntry != m_dependencies.end())
        return std::nullopt;

    m_dependencies.emplace(assetInfo);

    return std::nullopt;
}

std::optional<scr_string_t> AssetInfoCollector::Visit_ScriptString(scr_string_t scriptString)
{
    assert(scriptString < m_zone.m_script_strings.Count());

    if (scriptString >= m_zone.m_script_strings.Count())
        return std::nullopt;

    m_used_script_strings.emplace(scriptString);

    return std::nullopt;
}

void AssetInfoCollector::Visit_IndirectAssetRef(asset_type_t assetType, const char* assetName)
{
    m_indirect_asset_references.emplace(assetType, assetName);
}
