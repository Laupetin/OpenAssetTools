#include "GameT6.h"
#include "T6.h"

using namespace T6;

GameT6 g_GameT6;

void GameT6::AddZone(Zone* zone)
{
    m_zones.push_back(zone);
}

void GameT6::RemoveZone(Zone* zone)
{
    const auto foundEntry = std::find(m_zones.begin(), m_zones.end(), zone);

    if (foundEntry != m_zones.end())
        m_zones.erase(foundEntry);
}

std::vector<Zone*> GameT6::GetZones()
{
    return m_zones;
}