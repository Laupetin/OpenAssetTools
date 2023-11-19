#include "ZoneAssetPools.h"

ZoneAssetPools::ZoneAssetPools(Zone* zone)
    : m_zone(zone)
{
}

XAssetInfoGeneric* ZoneAssetPools::AddAsset(
    const asset_type_t type, std::string name, void* asset, std::vector<XAssetInfoGeneric*> dependencies, std::vector<scr_string_t> usedScriptStrings)
{
    return AddAsset(type, std::move(name), asset, std::move(dependencies), std::move(usedScriptStrings), m_zone);
}

XAssetInfoGeneric* ZoneAssetPools::AddAsset(const asset_type_t type,
                                            std::string name,
                                            void* asset,
                                            std::vector<XAssetInfoGeneric*> dependencies,
                                            std::vector<scr_string_t> usedScriptStrings,
                                            Zone* zone)
{
    auto* assetInfo = AddAssetToPool(type, std::move(name), asset, std::move(dependencies), std::move(usedScriptStrings), zone);
    if (assetInfo)
    {
        m_assets_in_order.push_back(assetInfo);
    }

    return assetInfo;
}

size_t ZoneAssetPools::GetTotalAssetCount() const
{
    return m_assets_in_order.size();
}

ZoneAssetPools::iterator ZoneAssetPools::begin() const
{
    return m_assets_in_order.begin();
}

ZoneAssetPools::iterator ZoneAssetPools::end() const
{
    return m_assets_in_order.end();
}
