#include "AssetWriter.h"

#include <cassert>

AssetWriter::AssetWriter(XAssetInfoGeneric* asset, const Zone& zone, ZoneOutputStream& stream)
    : ContentWriterBase(zone, stream),
      m_asset(asset),
      varScriptString(nullptr),
      varScriptStringWritten(nullptr)
{
}

const char* AssetWriter::NonReferenceAssetName(const char* assetName)
{
    if (assetName && assetName[0] == ',')
        return &assetName[1];

    return assetName;
}

scr_string_t AssetWriter::UseScriptString(const scr_string_t scrString) const
{
    assert(scrString < m_asset->m_zone->m_script_strings.Count());

    if (m_asset->m_zone == &m_zone)
        return scrString;

    // The asset comes from a different zone, we need to translate it
    const auto strValue = m_asset->m_zone->m_script_strings.CValue(scrString);
    return m_zone.m_script_strings.GetScriptString(strValue);
}

void AssetWriter::WriteScriptStringArray(const bool atStreamStart, const size_t count)
{
    if (atStreamStart)
    {
        assert(varScriptString != nullptr);
        varScriptStringWritten = m_stream->Write<scr_string_t>(varScriptString, count);
    }

    assert(varScriptStringWritten.Offset() != nullptr);

    for (size_t index = 0; index < count; index++)
    {
        *static_cast<scr_string_t*>(varScriptStringWritten.Offset()) = UseScriptString(*varScriptString);

        varScriptString++;
        varScriptStringWritten.Inc(sizeof(scr_string_t));
    }
}
