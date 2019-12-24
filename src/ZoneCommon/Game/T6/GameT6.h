#pragma once
#include "Game/IGame.h"

class GameT6 : public IGame
{
    std::vector<Zone*> m_zones;

public:
    void AddZone(Zone* zone) override;
    void RemoveZone(Zone* zone) override;
    std::vector<Zone*> GetZones() override;
};

extern GameT6 game_t6;