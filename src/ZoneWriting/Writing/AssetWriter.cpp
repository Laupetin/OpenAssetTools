#include "AssetWriter.h"

#include <cassert>
#include <algorithm>
#include <iterator>

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
    const auto scrStringEntry = std::find(m_zone->m_script_strings.begin(), m_zone->m_script_strings.end(), strValue);
    assert(scrStringEntry != m_zone->m_script_strings.end());

    if (scrStringEntry != m_zone->m_script_strings.end())
    {
        return static_cast<scr_string_t>(std::distance(scrStringEntry, m_zone->m_script_strings.begin()));
    }

    return 0;
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
