#pragma once

#include "Game/IGame.h"
#include "Zone.h"

#include <vector>

class ZoneRegistry
{
public:
    void AddZone(Zone* zone);
    void RemoveZone(Zone* zone);

    const std::vector<Zone*>& Zones() const;

    static ZoneRegistry* GetRegistryForGame(GameId gameId);

private:
    ZoneRegistry();

    std::vector<Zone*> m_zones;
};
