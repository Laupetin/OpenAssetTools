#pragma once

#include "ContentLoaderBase.h"
#include "Pool/XAssetInfo.h"
#include "Zone/ZoneTypes.h"

#include <unordered_set>
#include <vector>

class AssetLoader : public ContentLoaderBase
{
    asset_type_t m_asset_type;

protected:
    scr_string_t* varScriptString;

    AssetLoader(asset_type_t assetType, Zone* zone, IZoneInputStream* stream);

    XAssetInfoGeneric* LinkAsset(std::string name,
                                 void* asset,
                                 std::vector<XAssetInfoGeneric*> dependencies,
                                 std::vector<scr_string_t> scriptStrings,
                                 std::vector<IndirectAssetReference> indirectAssetReferences) const;

    _NODISCARD XAssetInfoGeneric* GetAssetInfo(const std::string& name) const;
};
