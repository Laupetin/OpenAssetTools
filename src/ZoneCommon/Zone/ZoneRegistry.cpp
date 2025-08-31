#include "ZoneRegistry.h"

#include <algorithm>
#include <cassert>
#include <memory>

ZoneRegistry::ZoneRegistry() = default;

void ZoneRegistry::AddZone(Zone* zone)
{
    assert(zone);

    m_zones.emplace_back(zone);
}

void ZoneRegistry::RemoveZone(Zone* zone)
{
    assert(zone);

    std::erase_if(m_zones,
                  [zone](const Zone* existingZone)
                  {
                      return existingZone == zone;
                  });
}

const std::vector<Zone*>& ZoneRegistry::Zones() const
{
    return m_zones;
}

ZoneRegistry* ZoneRegistry::GetRegistryForGame(GameId gameId)
{
    static ZoneRegistry registries[static_cast<unsigned>(GameId::COUNT)];

    assert(static_cast<unsigned>(gameId) < static_cast<unsigned>(GameId::COUNT));
    return &registries[static_cast<unsigned>(gameId)];
}
