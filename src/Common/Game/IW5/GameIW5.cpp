#include "GameIW5.h"

#include "IW5.h"

#include <algorithm>

using namespace IW5;

GameIW5 g_GameIW5;

GameId GameIW5::GetId()
{
    return GameId::IW5;
}

std::string GameIW5::GetFullName()
{
    return "Call Of Duty: Modern Warfare 3";
}

std::string GameIW5::GetShortName()
{
    return "IW5";
}

void GameIW5::AddZone(Zone* zone)
{
    m_zones.push_back(zone);
}

void GameIW5::RemoveZone(Zone* zone)
{
    const auto foundEntry = std::ranges::find(m_zones, zone);

    if (foundEntry != m_zones.end())
        m_zones.erase(foundEntry);
}

std::vector<Zone*> GameIW5::GetZones()
{
    return m_zones;
}

const std::vector<GameLanguagePrefix>& GameIW5::GetLanguagePrefixes()
{
    static std::vector<GameLanguagePrefix> prefixes;
    return prefixes;
}
