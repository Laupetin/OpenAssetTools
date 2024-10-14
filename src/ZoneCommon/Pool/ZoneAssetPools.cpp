#include "ZoneAssetPools.h"

#include "Game/IW3/GameAssetPoolIW3.h"
#include "Game/IW4/GameAssetPoolIW4.h"
#include "Game/IW5/GameAssetPoolIW5.h"
#include "Game/T5/GameAssetPoolT5.h"
#include "Game/T6/GameAssetPoolT6.h"
#include "Game/T6/ZoneConstantsT6.h"

#include <cassert>
#include <format>

ZoneAssetPools::ZoneAssetPools(Zone* zone)
    : m_zone(zone)
{
}

XAssetInfoGeneric* ZoneAssetPools::AddAsset(const asset_type_t type,
                                            std::string name,
                                            void* asset,
                                            std::vector<XAssetInfoGeneric*> dependencies,
                                            std::vector<scr_string_t> usedScriptStrings,
                                            std::vector<IndirectAssetReference> indirectAssetReferences)
{
    return AddAsset(std::make_unique<XAssetInfoGeneric>(
        type, std::move(name), asset, std::move(dependencies), std::move(usedScriptStrings), std::move(indirectAssetReferences), m_zone));
}

XAssetInfoGeneric* ZoneAssetPools::AddAsset(std::unique_ptr<XAssetInfoGeneric> xAssetInfo)
{
    auto* assetInfo = AddAssetToPool(std::move(xAssetInfo));
    if (assetInfo)
        m_assets_in_order.push_back(assetInfo);

    return assetInfo;
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

ZoneAssetPools::iterator ZoneAssetPools::begin() const
{
    return m_assets_in_order.begin();
}

ZoneAssetPools::iterator ZoneAssetPools::end() const
{
    return m_assets_in_order.end();
}

std::unique_ptr<ZoneAssetPools> ZoneAssetPools::CreateForGame(const GameId game, Zone* zone, const zone_priority_t priority)
{
    switch (game)
    {
    case GameId::IW3:
        return std::make_unique<GameAssetPoolIW3>(zone, priority);
    case GameId::IW4:
        return std::make_unique<GameAssetPoolIW4>(zone, priority);
    case GameId::IW5:
        return std::make_unique<GameAssetPoolIW5>(zone, priority);
    case GameId::T5:
        return std::make_unique<GameAssetPoolT5>(zone, priority);
    case GameId::T6:
        return std::make_unique<GameAssetPoolT6>(zone, priority);
    default:
        assert(false);
        return nullptr;
    }
}
