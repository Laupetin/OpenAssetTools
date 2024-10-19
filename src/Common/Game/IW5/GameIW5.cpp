#include "GameIW5.h"

#include <algorithm>

using namespace IW5;

GameId Game::GetId() const
{
    return GameId::IW5;
}

const std::string& Game::GetFullName() const
{
    static std::string fullName = "Call Of Duty: Modern Warfare 3";
    return fullName;
}

const std::string& Game::GetShortName() const
{
    static std::string shortName = "IW5";
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
