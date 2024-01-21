#pragma once

#include "ContentLoaderBase.h"
#include "Pool/XAssetInfo.h"
#include "Utils/ClassUtils.h"
#include "Zone/ZoneTypes.h"

#include <unordered_set>

class AssetMarker
{
    asset_type_t m_asset_type;

    std::unordered_set<XAssetInfoGeneric*> m_dependencies;
    std::unordered_set<scr_string_t> m_used_script_strings;

protected:
    AssetMarker(asset_type_t assetType, Zone* zone);

    void AddDependency(XAssetInfoGeneric* assetInfo);

    void Mark_ScriptString(scr_string_t scrString);
    void MarkArray_ScriptString(const scr_string_t* scrStringArray, size_t count);

    _NODISCARD XAssetInfoGeneric* GetAssetInfoByName(std::string name) const;

    Zone* m_zone;

public:
    _NODISCARD std::vector<XAssetInfoGeneric*> GetDependencies() const;
    _NODISCARD std::vector<scr_string_t> GetUsedScriptStrings() const;
};
