#include "Zone.h"

Zone::Zone(std::string name, const zone_priority_t priority, IGame* game)
    : m_memory(std::make_unique<ZoneMemory>()),
      m_registered(false),
      m_name(std::move(name)),
      m_priority(priority),
      m_language(GameLanguage::LANGUAGE_NONE),
      m_game(game),
      m_pools(ZoneAssetPools::CreateForGame(game->GetId(), this, priority))
{
    const auto assetTypeCount = m_pools->GetAssetTypeCount();
    for (auto i = 0; i < assetTypeCount; i++)
        m_pools->InitPoolDynamic(i);
}

Zone::~Zone()
{
    if (m_registered)
    {
        m_game->RemoveZone(this);
    }
}

void Zone::Register()
{
    if (!m_registered)
    {
        m_game->AddZone(this);
        m_registered = true;
    }
}

ZoneMemory* Zone::GetMemory() const
{
    return m_memory.get();
}
