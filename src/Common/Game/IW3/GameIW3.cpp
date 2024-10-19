#include "GameIW3.h"

#include <algorithm>

using namespace IW3;

GameId Game::GetId() const
{
    return GameId::IW3;
}

const std::string& Game::GetFullName() const
{
    static std::string fullName = "Call Of Duty 4: Modern Warfare";
    return fullName;
}

const std::string& Game::GetShortName() const
{
    static std::string shortName = "IW3";
    return shortName;
}

void Game::AddZone(Zone* zone)
{
    m_zones.push_back(zone);
}

void Game::RemoveZone(Zone* zone)
{
    const auto foundEntry = std::ranges::find(m_zones, zone);

    if (foundEntry != m_zones.end())
        m_zones.erase(foundEntry);
}

const std::vector<Zone*>& Game::GetZones() const
{
    return m_zones;
}

const std::vector<GameLanguagePrefix>& Game::GetLanguagePrefixes() const
{
    static std::vector<GameLanguagePrefix> prefixes;
    return prefixes;
}
