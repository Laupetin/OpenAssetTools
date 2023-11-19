#pragma once
#include "GameLanguage.h"

#include <vector>

class Zone;

class IGame
{
public:
    IGame() = default;
    virtual ~IGame() = default;
    IGame(const IGame& other) = default;
    IGame(IGame&& other) noexcept = default;
    IGame& operator=(const IGame& other) = default;
    IGame& operator=(IGame&& other) noexcept = default;

    virtual std::string GetFullName() = 0;
    virtual std::string GetShortName() = 0;
    virtual void AddZone(Zone* zone) = 0;
    virtual void RemoveZone(Zone* zone) = 0;
    virtual std::vector<Zone*> GetZones() = 0;
    virtual std::vector<GameLanguagePrefix> GetLanguagePrefixes() = 0;
};
