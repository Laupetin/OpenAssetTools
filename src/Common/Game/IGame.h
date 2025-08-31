#pragma once

#include "GameLanguage.h"

#include <type_traits>
#include <vector>

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

    [[nodiscard]] virtual GameId GetId() const = 0;
    [[nodiscard]] virtual const std::string& GetFullName() const = 0;
    [[nodiscard]] virtual const std::string& GetShortName() const = 0;
    [[nodiscard]] virtual const std::vector<GameLanguagePrefix>& GetLanguagePrefixes() const = 0;

    static IGame* GetGameById(GameId gameId);
};
