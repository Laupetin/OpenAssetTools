#include "AssetWriter.h"

#include <cassert>

AssetWriter::AssetWriter(XAssetInfoGeneric* asset, Zone* zone, IZoneOutputStream* stream)
    : ContentWriterBase(zone, stream),
      m_asset(asset),
      varScriptString(nullptr)
{
}

scr_string_t AssetWriter::UseScriptString(const scr_string_t scrString) const
{
    assert(scrString < m_asset->m_zone->m_script_strings.Count());

    if (m_asset->m_zone == m_zone)
        return scrString;

    const auto strValue = m_asset->m_zone->m_script_strings[scrString];
    return m_zone->m_script_strings.GetScriptString(strValue);
}

void AssetWriter::WriteScriptStringArray(const bool atStreamStart, const size_t count)
{
    assert(varScriptString != nullptr);

    if (atStreamStart)
        varScriptString = m_stream->Write<scr_string_t>(varScriptString, count);

    auto* ptr = varScriptString;
    for (size_t index = 0; index < count; index++)
    {
        *ptr = UseScriptString(*ptr);
        ptr++;
    }
}
