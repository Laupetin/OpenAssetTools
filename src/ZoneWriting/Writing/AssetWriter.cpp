#include "AssetWriter.h"

#include <cassert>

AssetWriter::AssetWriter(XAssetInfoGeneric* asset, Zone* zone, IZoneOutputStream* stream)
    : ContentWriterBase(zone, stream),
      m_asset(asset),
      varScriptString(nullptr),
      varScriptStringWritten(nullptr)
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
    if (atStreamStart)
    {
        assert(varScriptString != nullptr);
        varScriptStringWritten = m_stream->Write<scr_string_t>(varScriptString, count);
    }

    assert(varScriptStringWritten != nullptr);

    auto* ptr = varScriptStringWritten;
    for (size_t index = 0; index < count; index++)
    {
        *ptr = UseScriptString(*ptr);
        ptr++;
    }
}
