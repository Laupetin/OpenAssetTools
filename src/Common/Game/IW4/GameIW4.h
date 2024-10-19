#pragma once
#include "Game/IGame.h"

class GameIW4 final : public IGame
{
public:
    GameId GetId() override;
    std::string GetFullName() override;
    std::string GetShortName() override;
    void AddZone(Zone* zone) override;
    void RemoveZone(Zone* zone) override;
    std::vector<Zone*> GetZones() override;
    const std::vector<GameLanguagePrefix>& GetLanguagePrefixes() override;

private:
    std::vector<Zone*> m_zones;
};

extern GameIW4 g_GameIW4;
