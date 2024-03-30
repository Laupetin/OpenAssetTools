#include "AssetLoader.h"

#include <algorithm>
#include <cassert>

AssetLoader::AssetLoader(const asset_type_t assetType, Zone* zone, IZoneInputStream* stream)
    : ContentLoaderBase(zone, stream),
      m_asset_type(assetType),
      varScriptString(nullptr)
{
}

XAssetInfoGeneric* AssetLoader::LinkAsset(std::string name,
                                          void* asset,
                                          std::vector<XAssetInfoGeneric*> dependencies,
                                          std::vector<scr_string_t> scriptStrings,
                                          std::vector<IndirectAssetReference> indirectAssetReferences) const
{
    return m_zone->m_pools->AddAsset(
        m_asset_type, std::move(name), asset, std::move(dependencies), std::move(scriptStrings), std::move(indirectAssetReferences));
}

XAssetInfoGeneric* AssetLoader::GetAssetInfo(const std::string& name) const
{
    return m_zone->m_pools->GetAsset(m_asset_type, name);
}
