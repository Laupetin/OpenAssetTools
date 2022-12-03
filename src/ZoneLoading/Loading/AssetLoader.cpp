#include "AssetLoader.h"

#include <cassert>
#include <algorithm>

AssetLoader::AssetLoader(const asset_type_t assetType, Zone* zone, IZoneInputStream* stream)
    : ContentLoaderBase(zone, stream),
      m_asset_type(assetType),
      varScriptString(nullptr)
{
}

void AssetLoader::AddDependency(XAssetInfoGeneric* assetInfo)
{
    if (assetInfo == nullptr)
        return;

    const auto existingEntry = std::find(m_dependencies.begin(), m_dependencies.end(), assetInfo);
    if (existingEntry != m_dependencies.end())
    {
        return;
    }

    m_dependencies.push_back(assetInfo);
}

scr_string_t AssetLoader::UseScriptString(const scr_string_t scrString)
{
    assert(scrString < m_zone->m_script_strings.Count());

    if (scrString >= m_zone->m_script_strings.Count())
        return 0u;

    m_used_script_strings.emplace(scrString);
    return scrString;
}

void AssetLoader::LoadScriptStringArray(const bool atStreamStart, const size_t count)
{
    assert(varScriptString != nullptr);

    if (atStreamStart)
        m_stream->Load<scr_string_t>(varScriptString, count);

    auto* ptr = varScriptString;
    for (size_t index = 0; index < count; index++)
    {
        *ptr = UseScriptString(*ptr);
        ptr++;
    }
}

void AssetLoader::MarkScriptStringArrayAsUsed(const scr_string_t* scrStringArray, const size_t count)
{
    for (size_t index = 0; index < count; index++)
    {
        const auto scrString = scrStringArray[index];
        if (scrString >= m_zone->m_script_strings.Count())
            continue;

        m_used_script_strings.emplace(scrString);
    }
}

XAssetInfoGeneric* AssetLoader::LinkAsset(std::string name, void* asset)
{
    std::vector<scr_string_t> usedScriptStrings;
    if (!m_used_script_strings.empty())
    {
        for (auto scrString : m_used_script_strings)
        {
            usedScriptStrings.push_back(scrString);
        }

        std::sort(usedScriptStrings.begin(), usedScriptStrings.end());
        m_used_script_strings.clear();
    }

    return m_zone->m_pools->AddAsset(m_asset_type, std::move(name), asset, std::move(m_dependencies), std::move(usedScriptStrings));
}

XAssetInfoGeneric* AssetLoader::GetAssetInfo(std::string name) const
{
    return m_zone->m_pools->GetAsset(m_asset_type, std::move(name));
}
