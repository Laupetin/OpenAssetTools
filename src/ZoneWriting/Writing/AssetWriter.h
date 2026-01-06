#pragma once

#include "ContentWriterBase.h"
#include "Pool/XAssetInfo.h"
#include "Zone/Zone.h"
#include "Zone/ZoneTypes.h"

class AssetWriter : public ContentWriterBase
{
protected:
    AssetWriter(XAssetInfoGeneric* asset, const Zone& zone, ZoneOutputStream& stream);

    [[nodiscard]] static const char* NonReferenceAssetName(const char* assetName);
    [[nodiscard]] scr_string_t UseScriptString(scr_string_t scrString) const;
    void WriteScriptStringArray(bool atStreamStart, size_t count);

    XAssetInfoGeneric* m_asset;

    scr_string_t* varScriptString;
    ZoneOutputOffset varScriptStringWritten;
};
