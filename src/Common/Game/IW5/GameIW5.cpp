#include "GameIW5.h"

#include <algorithm>

using namespace IW5;

GameId Game::GetId() const
{
    return GameId::IW5;
}

const std::string& Game::GetFullName() const
{
    static std::string fullName = "Call Of Duty: Modern Warfare 3";
    return fullName;
}

const std::string& Game::GetShortName() const
{
    static std::string shortName = "IW5";
    return shortName;
}

const std::vector<GameLanguagePrefix>& Game::GetLanguagePrefixes() const
{
    static std::vector<GameLanguagePrefix> prefixes;
    return prefixes;
}
