#include "AssetLoader.h"

#include <cassert>
#include <algorithm>

AssetLoader::AssetLoader(const asset_type_t assetType, Zone* zone, IZoneInputStream* stream)
    : ContentLoaderBase(zone, stream),
      m_asset_type(assetType),
      varScriptString(nullptr)
{
    m_asset_type = assetType;
    m_zone = zone;
    m_stream = stream;
    varScriptString = nullptr;
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
    assert(scrString < m_zone->m_script_strings.size());

    if (scrString >= m_zone->m_script_strings.size())
        return 0u;

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

XAssetInfoGeneric* AssetLoader::LinkAsset(std::string name, void* asset)
{
    return m_zone->m_pools->AddAsset(m_asset_type, std::move(name), asset, m_dependencies);
}

XAssetInfoGeneric* AssetLoader::GetAssetInfo(std::string name) const
{
    return m_zone->m_pools->GetAsset(m_asset_type, std::move(name));
}
