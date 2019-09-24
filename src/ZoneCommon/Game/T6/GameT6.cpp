#include "GameT6.h"
#include "T6.h"

using namespace T6;

GameT6 game_t6;

void GameT6::AddZone(Zone* zone)
{
    m_zones.push_back(zone);
}

std::vector<Zone*> GameT6::GetZones()
{
    return m_zones;
}