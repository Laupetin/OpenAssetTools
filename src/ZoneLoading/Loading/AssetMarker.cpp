#include "AssetMarker.h"

#include <algorithm>
#include <cassert>

AssetMarker::AssetMarker(const asset_type_t assetType, Zone* zone)
    : m_asset_type(assetType),
      m_zone(zone)
{
}

void AssetMarker::AddDependency(XAssetInfoGeneric* assetInfo)
{
    if (assetInfo == nullptr)
        return;

    const auto existingEntry = m_dependencies.find(assetInfo);
    if (existingEntry != m_dependencies.end())
        return;

    m_dependencies.emplace(assetInfo);
}

void AssetMarker::Mark_ScriptString(const scr_string_t scrString)
{
    assert(scrString < m_zone->m_script_strings.Count());

    if (scrString >= m_zone->m_script_strings.Count())
        return;

    m_used_script_strings.emplace(scrString);
}

void AssetMarker::MarkArray_ScriptString(const scr_string_t* scrStringArray, const size_t count)
{
    assert(scrStringArray != nullptr);

    for (size_t index = 0; index < count; index++)
        Mark_ScriptString(scrStringArray[index]);
}

void AssetMarker::Mark_IndirectAssetRef(asset_type_t type, const char* assetRefName)
{
    if (!assetRefName || !assetRefName[0])
        return;

    m_indirect_asset_references.emplace(type, assetRefName);
}

void AssetMarker::MarkArray_IndirectAssetRef(const asset_type_t type, const char** assetRefNames, const size_t count)
{
    assert(assetRefNames != nullptr);

    for (size_t index = 0; index < count; index++)
        Mark_IndirectAssetRef(type, assetRefNames[index]);
}

XAssetInfoGeneric* AssetMarker::GetAssetInfoByName(std::string name) const
{
    return m_zone->m_pools->GetAsset(m_asset_type, std::move(name));
}

std::vector<XAssetInfoGeneric*> AssetMarker::GetDependencies() const
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

std::vector<scr_string_t> AssetMarker::GetUsedScriptStrings() const
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

std::vector<IndirectAssetReference> AssetMarker::GetIndirectAssetReferences() const
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
