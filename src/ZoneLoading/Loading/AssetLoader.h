#pragma once

#include "ContentLoaderBase.h"
#include "Pool/XAssetInfo.h"
#include "Zone/ZoneTypes.h"

#include <vector>

class AssetLoader : public ContentLoaderBase
{
protected:
    AssetLoader(asset_type_t assetType, Zone& zone, ZoneInputStream& stream);

    XAssetInfoGeneric* LinkAsset(std::string name,
                                 void* asset,
                                 std::vector<XAssetInfoGeneric*> dependencies,
                                 std::vector<scr_string_t> scriptStrings,
                                 std::vector<IndirectAssetReference> indirectAssetReferences) const;

    [[nodiscard]] XAssetInfoGeneric* GetAssetInfo(const std::string& name) const;

    scr_string_t* varScriptString;

private:
    asset_type_t m_asset_type;
};
