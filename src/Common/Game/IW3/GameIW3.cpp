#include "GameIW3.h"

#include "IW3.h"

#include <algorithm>

using namespace IW3;

GameIW3 g_GameIW3;

std::string GameIW3::GetFullName()
{
    return "Call Of Duty 4: Modern Warfare";
}

std::string GameIW3::GetShortName()
{
    return "IW3";
}

void GameIW3::AddZone(Zone* zone)
{
    m_zones.push_back(zone);
}

void GameIW3::RemoveZone(Zone* zone)
{
    const auto foundEntry = std::ranges::find(m_zones, zone);

    if (foundEntry != m_zones.end())
        m_zones.erase(foundEntry);
}

std::vector<Zone*> GameIW3::GetZones()
{
    return m_zones;
}

std::vector<GameLanguagePrefix> GameIW3::GetLanguagePrefixes()
{
    std::vector<GameLanguagePrefix> prefixes;
    return prefixes;
}
