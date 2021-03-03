#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Zone/ZoneTypes.h"
#include "Pool/ZoneAssetPools.h"
#include "Game/IGame.h"
#include "Game/GameLanguage.h"
#include "Zone/XBlock.h"
#include "ZoneMemory.h"

class IGame;
class ZoneAssetPools;

class Zone
{
    std::vector<XBlock*> m_blocks;
    std::unique_ptr<ZoneMemory> m_memory;

    bool m_registered;

public:
    std::string m_name;
    zone_priority_t m_priority;
    GameLanguage m_language;
    IGame* m_game;
    std::vector<std::string> m_script_strings;
    std::unique_ptr<ZoneAssetPools> m_pools;

    Zone(std::string name, zone_priority_t priority, IGame* game);
    ~Zone();

    void Register();

    ZoneMemory* GetMemory() const;
};
