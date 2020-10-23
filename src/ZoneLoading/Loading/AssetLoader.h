#pragma once
#include "Pool/XAssetInfo.h"
#include "ContentLoaderBase.h"
#include "IZoneScriptStringProvider.h"

class AssetLoader : public ContentLoaderBase
{
    asset_type_t m_asset_type;

    std::vector<std::string> m_used_script_strings;

    std::vector<XAssetInfoGeneric*> m_dependencies;

protected:
    IZoneScriptStringProvider* m_script_string_provider;

    scr_string_t* varScriptString;

    AssetLoader(asset_type_t assetType, IZoneScriptStringProvider* scriptStringProvider, Zone* zone, IZoneInputStream* stream);
    
    void AddDependency(XAssetInfoGeneric* assetInfo);

    scr_string_t UseScriptString(scr_string_t scrString);
    void LoadScriptStringArray(bool atStreamStart, size_t count);
    void LoadScriptStringArrayRealloc(bool atStreamStart, size_t count);

    XAssetInfoGeneric* LinkAsset(std::string name, void* asset);

    XAssetInfoGeneric* GetAssetInfo(std::string name) const;
};
