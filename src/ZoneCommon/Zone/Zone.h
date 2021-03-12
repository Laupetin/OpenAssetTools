#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Zone/ZoneTypes.h"
#include "Pool/ZoneAssetPools.h"
#include "Game/IGame.h"
#include "Game/GameLanguage.h"
#include "ZoneMemory.h"

class IGame;
class ZoneAssetPools;

class Zone
{
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
    Zone(const Zone& other) = delete;
    Zone(Zone&& other) noexcept = default;
    Zone& operator=(const Zone& other) = delete;
    Zone& operator=(Zone&& other) noexcept = default;

    void Register();

    ZoneMemory* GetMemory() const;
};
