#include "BaseAssetMarker.h"

#include <algorithm>
#include <cassert>

BaseAssetMarker::BaseAssetMarker(AssetVisitor& visitor)
    : m_visitor(visitor)
{
}

void BaseAssetMarker::Mark_ScriptString(scr_string_t& scriptString) const
{
    const auto result = m_visitor.Visit_ScriptString(scriptString);
    if (result.has_value())
        scriptString = *result;
}

void BaseAssetMarker::MarkArray_ScriptString(scr_string_t* scriptStringArray, const size_t count) const
{
    assert(scriptStringArray != nullptr);

    for (size_t index = 0; index < count; index++)
        Mark_ScriptString(scriptStringArray[index]);
}

void BaseAssetMarker::Mark_IndirectAssetRef(const asset_type_t assetType, const char* assetName) const
{
    if (!assetName)
        return;

    m_visitor.Visit_IndirectAssetRef(assetType, assetName);
}

void BaseAssetMarker::MarkArray_IndirectAssetRef(const asset_type_t assetType, const char** assetNames, const size_t count) const
{
    assert(assetNames != nullptr);

    for (size_t index = 0; index < count; index++)
        Mark_IndirectAssetRef(assetType, assetNames[index]);
}
