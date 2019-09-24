#include "Zone.h"

Zone::Zone(std::string name, const zone_priority_t priority, IZoneAssetPools* pools, IGame* game)
{
    m_name = std::move(name);
    m_priority = priority;
    m_pools = pools;
    m_game = game;
}

IZoneAssetPools* Zone::GetPools() const
{
    return m_pools;
}
