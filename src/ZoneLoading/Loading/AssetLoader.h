#pragma once

#include "Zone/ZoneTypes.h"
#include "Pool/XAssetInfo.h"
#include "ContentLoaderBase.h"

class AssetLoader : public ContentLoaderBase
{
    asset_type_t m_asset_type;

    std::vector<XAssetInfoGeneric*> m_dependencies;

protected:
    scr_string_t* varScriptString;

    AssetLoader(asset_type_t assetType, Zone* zone, IZoneInputStream* stream);
    
    void AddDependency(XAssetInfoGeneric* assetInfo);

    scr_string_t UseScriptString(scr_string_t scrString);
    void LoadScriptStringArray(bool atStreamStart, size_t count);

    XAssetInfoGeneric* LinkAsset(std::string name, void* asset);

    XAssetInfoGeneric* GetAssetInfo(std::string name) const;
};
