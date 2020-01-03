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

void AssetLoader::AddDependency(const asset_type_t type, std::string& name)
{
    for(auto& existingDependency : m_dependencies)
    {
        if(existingDependency.m_type == type
            && existingDependency.m_name == name)
        {
            return;
        }
    }

    XAssetDependency dependency;
    dependency.m_type = type;
    dependency.m_name = name;

    m_dependencies.push_back(dependency);
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
    m_used_script_strings.push_back(scrStringValue);

    return scriptStringIndex;
}

void AssetLoader::LoadScriptStringArray(const bool atStreamStart, const size_t count)
{
    assert(varScriptString != nullptr);

    if (atStreamStart)
        m_stream->Load<scr_string_t>(varScriptString, count);

    for (size_t index = 0; index < count; index++)
    {
        *varScriptString = UseScriptString(*varScriptString);
        varScriptString++;
    }
}

void* AssetLoader::LinkAsset(std::string name, void* asset)
{
    return m_zone->GetPools()->AddAsset(m_asset_type, std::move(name), asset, m_used_script_strings, m_dependencies);
}