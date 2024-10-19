#include "GameIW4.h"

#include "IW4.h"

#include <algorithm>

using namespace IW4;

GameIW4 g_GameIW4;

GameId GameIW4::GetId()
{
    return GameId::IW4;
}

std::string GameIW4::GetFullName()
{
    return "Call Of Duty: Modern Warfare 2";
}

std::string GameIW4::GetShortName()
{
    return "IW4";
}

void GameIW4::AddZone(Zone* zone)
{
    m_zones.push_back(zone);
}

void GameIW4::RemoveZone(Zone* zone)
{
    const auto foundEntry = std::ranges::find(m_zones, zone);

    if (foundEntry != m_zones.end())
        m_zones.erase(foundEntry);
}

std::vector<Zone*> GameIW4::GetZones()
{
    return m_zones;
}

std::vector<GameLanguagePrefix> GameIW4::GetLanguagePrefixes()
{
    std::vector<GameLanguagePrefix> prefixes;
    return prefixes;
}
