#include "AssetPool.h"

#include "GlobalAssetPool.h"

#include <cassert>
#include <format>

AssetPool::Iterator::Iterator(std::unordered_map<std::string, XAssetInfoGeneric*>::iterator i)
    : m_iterator(std::move(i))
{
}

bool AssetPool::Iterator::operator!=(const Iterator& rhs) const
{
    return m_iterator != rhs.m_iterator;
}

XAssetInfoGeneric* AssetPool::Iterator::operator*() const
{
    return m_iterator.operator*().second;
}

void AssetPool::Iterator::operator++()
{
    ++m_iterator;
}

AssetPool::CIterator::CIterator(std::unordered_map<std::string, XAssetInfoGeneric*>::const_iterator i)
    : m_iterator(std::move(i))
{
}

bool AssetPool::CIterator::operator!=(const CIterator& rhs) const
{
    return m_iterator != rhs.m_iterator;
}

const XAssetInfoGeneric* AssetPool::CIterator::operator*() const
{
    return m_iterator.operator*().second;
}

void AssetPool::CIterator::operator++()
{
    ++m_iterator;
}

//
// AssetPool
//

XAssetInfoGeneric* AssetPool::AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo)
{
    const auto normalizedName = XAssetInfoGeneric::NormalizeAssetName(xAssetInfo->m_name);

    auto* pAssetInfo = xAssetInfo.get();
    m_asset_lookup[normalizedName] = pAssetInfo;
    m_assets.emplace_back(std::move(xAssetInfo));

    return pAssetInfo;
}

XAssetInfoGeneric* AssetPool::GetAsset(const std::string& name)
{
    const auto normalizedName = XAssetInfoGeneric::NormalizeAssetName(name);

    const auto foundAsset = m_asset_lookup.find(normalizedName);
    if (foundAsset == m_asset_lookup.end())
        return nullptr;

    return foundAsset->second;
}

size_t AssetPool::size() const
{
    return m_asset_lookup.size();
}

bool AssetPool::empty() const
{
    return m_asset_lookup.empty();
}

AssetPool::Iterator AssetPool::begin()
{
    return Iterator(m_asset_lookup.begin());
}

AssetPool::Iterator AssetPool::end()
{
    return Iterator(m_asset_lookup.end());
}

AssetPool::CIterator AssetPool::begin() const
{
    return CIterator(m_asset_lookup.cbegin());
}

AssetPool::CIterator AssetPool::end() const
{
    return CIterator(m_asset_lookup.cend());
}

//
// ZoneAssetPools
//

ZoneAssetPools::ZoneAssetPools(Zone& zone, const zone_priority_t priority)
    : m_zone(zone)
{
    const auto assetTypeCount = IGame::GetGameById(zone.m_game_id)->GetAssetTypeCount();
    const auto* gameGlobalAssetPools = GameGlobalAssetPools::GetGlobalPoolsForGame(zone.m_game_id);

    m_asset_pools.resize(assetTypeCount);
    for (asset_type_t assetType = 0; assetType < assetTypeCount; assetType++)
    {
        m_asset_pools[assetType] = std::make_unique<AssetPool>();
        gameGlobalAssetPools->LinkAssetPool(assetType, m_asset_pools[assetType].get(), priority);
    }
}

ZoneAssetPools::~ZoneAssetPools()
{
    const auto assetTypeCount = IGame::GetGameById(m_zone.m_game_id)->GetAssetTypeCount();
    const auto* gameGlobalAssetPools = GameGlobalAssetPools::GetGlobalPoolsForGame(m_zone.m_game_id);

    assert(assetTypeCount == m_asset_pools.size());
    for (asset_type_t assetType = 0; assetType < assetTypeCount; assetType++)
    {
        assert(m_asset_pools[assetType]);
        gameGlobalAssetPools->UnlinkAssetPool(assetType, m_asset_pools[assetType].get());
    }
}

XAssetInfoGeneric* ZoneAssetPools::AddAsset(const asset_type_t type,
                                            std::string name,
                                            void* asset,
                                            std::vector<XAssetInfoGeneric*> dependencies,
                                            std::vector<scr_string_t> usedScriptStrings,
                                            std::vector<IndirectAssetReference> indirectAssetReferences)
{
    return AddAsset(std::make_unique<XAssetInfoGeneric>(
        type, std::move(name), asset, std::move(dependencies), std::move(usedScriptStrings), std::move(indirectAssetReferences), &m_zone));
}

XAssetInfoGeneric* ZoneAssetPools::AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo)
{
    const auto type = xAssetInfo->m_type;
    assert(m_asset_pools.size() > type);

    auto* assetInfo = m_asset_pools[type]->AddAsset(std::move(xAssetInfo));

    const auto normalizedName = XAssetInfoGeneric::NormalizeAssetName(assetInfo->m_name);
    GameGlobalAssetPools::GetGlobalPoolsForGame(m_zone.m_game_id)->LinkAsset(type, m_asset_pools[type].get(), normalizedName, assetInfo);

    assert(assetInfo);
    m_assets_in_order.emplace_back(assetInfo);

    return assetInfo;
}

XAssetInfoGeneric* ZoneAssetPools::GetAsset(const asset_type_t type, const std::string& name) const
{
    assert(m_asset_pools.size() > type);

    return m_asset_pools[type]->GetAsset(name);
}

XAssetInfoGeneric* ZoneAssetPools::GetAssetOrAssetReference(const asset_type_t type, const std::string& name) const
{
    auto* result = GetAsset(type, name);

    if (result != nullptr || (!name.empty() && name[0] == ','))
        return result;

    result = GetAsset(type, std::format(",{}", name));
    return result;
}

size_t ZoneAssetPools::GetTotalAssetCount() const
{
    return m_assets_in_order.size();
}

ZoneAssetPools::all_iterator ZoneAssetPools::begin() const
{
    return m_assets_in_order.begin();
}

ZoneAssetPools::all_iterator ZoneAssetPools::end() const
{
    return m_assets_in_order.end();
}
