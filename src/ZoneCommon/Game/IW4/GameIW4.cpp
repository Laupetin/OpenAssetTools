#include "GameIW4.h"

#include <algorithm>

#include "IW4.h"

using namespace IW4;

GameIW4 g_GameIW4;

const std::string GameIW4::NAME = "IW4";

const std::string& GameIW4::GetName()
{
    return NAME;
}

void GameIW4::AddZone(Zone* zone)
{
    m_zones.push_back(zone);
}

void GameIW4::RemoveZone(Zone* zone)
{
    const auto foundEntry = std::find(m_zones.begin(), m_zones.end(), zone);

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
