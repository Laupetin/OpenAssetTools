#pragma once

#include "ContentLoaderBase.h"
#include "Pool/XAssetInfo.h"
#include "Zone/ZoneTypes.h"

#include <unordered_set>
#include <vector>

class AssetLoader : public ContentLoaderBase
{
    asset_type_t m_asset_type;

    std::vector<XAssetInfoGeneric*> m_dependencies;
    std::unordered_set<scr_string_t> m_used_script_strings;

protected:
    scr_string_t* varScriptString;

    AssetLoader(asset_type_t assetType, Zone* zone, IZoneInputStream* stream);

    void AddDependency(XAssetInfoGeneric* assetInfo);

    scr_string_t UseScriptString(scr_string_t scrString);
    void LoadScriptStringArray(bool atStreamStart, size_t count);
    void MarkScriptStringArrayAsUsed(const scr_string_t* scrStringArray, size_t count);

    XAssetInfoGeneric* LinkAsset(std::string name, void* asset);

    XAssetInfoGeneric* GetAssetInfo(std::string name) const;
};
