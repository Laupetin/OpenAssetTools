#include "Zone.h"

#include "ZoneRegistry.h"

Zone::Zone(std::string name, const zone_priority_t priority, GameId gameId)
    : m_name(std::move(name)),
      m_priority(priority),
      m_language(GameLanguage::LANGUAGE_NONE),
      m_game_id(gameId),
      m_pools(ZoneAssetPools::CreateForGame(gameId, this, priority)),
      m_memory(std::make_unique<ZoneMemory>())
{
}

Zone::~Zone()
{
    if (m_registered)
    {
        ZoneRegistry::GetRegistryForGame(m_game_id)->RemoveZone(this);
    }
}

void Zone::Register()
{
    if (!m_registered)
    {
        ZoneRegistry::GetRegistryForGame(m_game_id)->AddZone(this);
        m_registered = true;
    }
}

ZoneMemory& Zone::Memory() const
{
    return *m_memory;
}
