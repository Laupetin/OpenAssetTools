#pragma once
#include "Game/IGame.h"

class GameIW3 : public IGame
{
    std::vector<Zone*> m_zones;

public:
    std::string GetFullName() override;
    std::string GetShortName() override;
    void AddZone(Zone* zone) override;
    void RemoveZone(Zone* zone) override;
    std::vector<Zone*> GetZones() override;
    std::vector<GameLanguagePrefix> GetLanguagePrefixes() override;
};

extern GameIW3 g_GameIW3;