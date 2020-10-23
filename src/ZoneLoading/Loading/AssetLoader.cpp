#include "AssetLoader.h"
#include <cassert>

AssetLoader::AssetLoader(const asset_type_t assetType, IZoneScriptStringProvider* scriptStringProvider, Zone* zone, IZoneInputStream* stream)
{
    m_asset_type = assetType;
    m_script_string_provider = scriptStringProvider;
    m_zone = zone;
    m_stream = stream;
    varScriptString = nullptr;
}

void AssetLoader::AddDependency(XAssetInfoGeneric* assetInfo)
{
    if (assetInfo == nullptr)
        return;

    const auto existingEntry = std::find(m_dependencies.begin(), m_dependencies.end(), assetInfo);
    if(existingEntry != m_dependencies.end())
    {
        return;
    }

    m_dependencies.push_back(assetInfo);
}

scr_string_t AssetLoader::UseScriptString(const scr_string_t scrString)
{
    std::string& scrStringValue = m_script_string_provider->GetZoneScriptString(scrString);

    scr_string_t scriptStringIndex = 0;
    for(auto& existingScriptString : m_used_script_strings)
    {
        if(existingScriptString == scrStringValue)
        {
            return scriptStringIndex;
        }

        scriptStringIndex++;
    }

    scriptStringIndex = static_cast<scr_string_t>(m_used_script_strings.size());

    // If an asset uses script strings make sure that script string 0 is always empty
    if(scriptStringIndex == 0 && !scrStringValue.empty())
    {
        m_used_script_strings.emplace_back("");
        scriptStringIndex++;
    }

    m_used_script_strings.push_back(scrStringValue);

    return scriptStringIndex;
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

void AssetLoader::LoadScriptStringArrayRealloc(const bool atStreamStart, const size_t count)
{
    assert(varScriptString != nullptr);

    if (atStreamStart)
        m_stream->Load<scr_string_t>(varScriptString, count);

    auto* scriptStringsNew = static_cast<scr_string_t*>(m_zone->GetMemory()->Alloc(sizeof scr_string_t * count));
    memcpy_s(scriptStringsNew, sizeof scr_string_t * count, varScriptString, sizeof scr_string_t * count);
    varScriptString = scriptStringsNew;

    auto* ptr = varScriptString;
    for (size_t index = 0; index < count; index++)
    {
        *ptr = UseScriptString(*ptr);
        ptr++;
    }
}

XAssetInfoGeneric* AssetLoader::LinkAsset(std::string name, void* asset)
{
    return m_zone->GetPools()->AddAsset(m_asset_type, std::move(name), asset, m_used_script_strings, m_dependencies);;
}

XAssetInfoGeneric* AssetLoader::GetAssetInfo(std::string name) const
{
    return m_zone->GetPools()->GetAsset(m_asset_type, std::move(name));
}
