#pragma once
#include "ZoneTypes.h"
#include "Pool/IZoneAssetPools.h"
#include "Game/IGame.h"
#include "Game/GameLanguage.h"
#include "Zone/XBlock.h"
#include "ZoneMemory.h"
#include <string>

class IGame;

class Zone
{
    IZoneAssetPools* m_pools;
    std::vector<XBlock*> m_blocks;
    ZoneMemory* m_memory;

    bool m_registered;

public:
    std::string m_name;
    zone_priority_t m_priority;
    GameLanguage m_language;
    IGame* m_game;

    Zone(std::string name, zone_priority_t priority, IZoneAssetPools* pools, IGame* game);
    ~Zone();

    void Register();

    IZoneAssetPools* GetPools() const;
    ZoneMemory* GetMemory() const;
};
