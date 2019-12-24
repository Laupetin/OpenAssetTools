#pragma once
#include "ZoneTypes.h"
#include "Pool/IZoneAssetPools.h"
#include "Game/IGame.h"
#include "Zone/XBlock.h"
#include "ZoneMemory.h"
#include <string>

class IGame;

enum class ZoneLanguage
{
    LANGUAGE_NONE,
    LANGUAGE_ENGLISH,
    LANGUAGE_FRENCH,
    LANGUAGE_GERMAN,
    LANGUAGE_ITALIAN,
    LANGUAGE_SPANISH,
    LANGUAGE_BRITISH,
    LANGUAGE_RUSSIAN,
    LANGUAGE_POLISH,
    LANGUAGE_KOREAN,
    LANGUAGE_TAIWANESE,
    LANGUAGE_JAPANESE,
    LANGUAGE_CHINESE,
    LANGUAGE_THAI,
    LANGUAGE_LEET,
    LANGUAGE_CZECH,
    LANGUAGE_FRENCH_CAN,
    LANGUAGE_AUSTRIAN,
    LANGUAGE_PORTUGUESE,
    LANGUAGE_MEXICAN_SPANISH,
    LANGUAGE_FULL_JAPANESE
};

class Zone
{
    IZoneAssetPools* m_pools;
    std::vector<XBlock*> m_blocks;
    ZoneMemory* m_memory;

    bool m_registered;

public:
    std::string m_name;
    zone_priority_t m_priority;
    ZoneLanguage m_language;
    IGame* m_game;

    Zone(std::string name, zone_priority_t priority, IZoneAssetPools* pools, IGame* game);
    ~Zone();

    void Register();

    IZoneAssetPools* GetPools() const;
    ZoneMemory* GetMemory() const;
};
