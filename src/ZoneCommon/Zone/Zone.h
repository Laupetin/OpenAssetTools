#pragma once
#include "ZoneTypes.h"
#include "Pool/IZoneAssetPools.h"
#include "Game/IGame.h"
#include <string>

class IGame;

class Zone
{
    std::string m_name;
    zone_priority_t m_priority;
    IZoneAssetPools* m_pools;

public:
    IGame* m_game;

    Zone(std::string name, zone_priority_t priority, IZoneAssetPools* pools, IGame* game);

    IZoneAssetPools* GetPools() const;
};
