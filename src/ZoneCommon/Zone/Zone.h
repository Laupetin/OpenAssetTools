#pragma once

#include "Game/GameLanguage.h"
#include "Game/IGame.h"
#include "Pool/AssetPool.h"
#include "Zone/ZoneTypes.h"
#include "ZoneMemory.h"
#include "ZoneScriptStrings.h"

#include <memory>
#include <string>

class Zone
{
public:
    Zone(std::string name, zone_priority_t priority, GameId gameId, GamePlatform platform);
    ~Zone();
    Zone(const Zone& other) = delete;
    Zone(Zone&& other) noexcept = delete;
    Zone& operator=(const Zone& other) = delete;
    Zone& operator=(Zone&& other) noexcept = delete;

    void Register();

    [[nodiscard]] ZoneMemory& Memory() const;

    std::string m_name;
    zone_priority_t m_priority;
    GameLanguage m_language;
    GameId m_game_id;
    GamePlatform m_platform;
    ZoneScriptStrings m_script_strings;
    ZoneAssetPools m_pools;

private:
    std::unique_ptr<ZoneMemory> m_memory;

    bool m_registered;
};
