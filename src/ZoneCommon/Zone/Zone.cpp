#include "Zone.h"

Zone::Zone(std::string name, const zone_priority_t priority, IZoneAssetPools* pools, IGame* game)
{
    m_name = std::move(name);
    m_priority = priority;
    m_pools = pools;
    m_game = game;
    m_language = ZoneLanguage::LANGUAGE_NONE;
    m_memory = new ZoneMemory();
    m_registered = false;
}

Zone::~Zone()
{
    if(m_registered)
    {
        m_game->RemoveZone(this);
    }

    delete m_pools;
    m_pools = nullptr;

    delete m_memory;
    m_memory = nullptr;
}

void Zone::Register()
{
    if (!m_registered)
    {
        m_game->AddZone(this);
        m_registered = true;
    }
}

IZoneAssetPools* Zone::GetPools() const
{
    return m_pools;
}

ZoneMemory* Zone::GetMemory() const
{
    return m_memory;
}
