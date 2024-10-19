#pragma once
#include "Game/IGame.h"

class GameT6 final : public IGame
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

extern GameT6 g_GameT6;
