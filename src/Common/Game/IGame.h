#pragma once
#include "GameLanguage.h"

#include <type_traits>
#include <vector>

class Zone;

enum class GameId
{
    IW3,
    IW4,
    IW5,
    T5,
    T6,

    COUNT
};

static constexpr const char* GameId_Names[]{
    "IW3",
    "IW4",
    "IW5",
    "T5",
    "T6",
};
static_assert(std::extent_v<decltype(GameId_Names)> == static_cast<unsigned>(GameId::COUNT));

class IGame
{
public:
    IGame() = default;
    virtual ~IGame() = default;
    IGame(const IGame& other) = default;
    IGame(IGame&& other) noexcept = default;
    IGame& operator=(const IGame& other) = default;
    IGame& operator=(IGame&& other) noexcept = default;

    virtual GameId GetId() = 0;
    virtual std::string GetFullName() = 0;
    virtual std::string GetShortName() = 0;
    virtual void AddZone(Zone* zone) = 0;
    virtual void RemoveZone(Zone* zone) = 0;
    virtual std::vector<Zone*> GetZones() = 0;
    virtual std::vector<GameLanguagePrefix> GetLanguagePrefixes() = 0;
};
