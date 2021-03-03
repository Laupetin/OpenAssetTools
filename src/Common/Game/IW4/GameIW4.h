#pragma once
#include "Game/IGame.h"

class GameIW4 : public IGame
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

extern GameIW4 g_GameIW4;