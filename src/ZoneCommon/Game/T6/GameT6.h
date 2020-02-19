#pragma once
#include "Game/IGame.h"

class GameT6 : public IGame
{
    static const std::string NAME;
    std::vector<Zone*> m_zones;

public:
    const std::string& GetName() override;
    void AddZone(Zone* zone) override;
    void RemoveZone(Zone* zone) override;
    std::vector<Zone*> GetZones() override;
    std::vector<GameLanguagePrefix> GetLanguagePrefixes() override;
};

extern GameT6 g_GameT6;