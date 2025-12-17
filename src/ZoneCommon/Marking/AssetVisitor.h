#pragma once

#include "Pool/XAssetInfo.h"
#include "Zone/ZoneTypes.h"

#include <optional>

class AssetVisitor
{
public:
    virtual ~AssetVisitor() = default;

    virtual std::optional<XAssetInfoGeneric> Visit_Dependency(asset_type_t assetType, const char* assetName)
    {
        // Do nothing by default
        return std::nullopt;
    }

    virtual std::optional<scr_string_t> Visit_ScriptString(scr_string_t scriptString)
    {
        // Do nothing by default
        return std::nullopt;
    }

    virtual void Visit_IndirectAssetRef(asset_type_t assetType, const char* assetName)
    {
        // Do nothing by default
    }
};
