#include "GameIW4.h"

#include <algorithm>

using namespace IW4;

GameId Game::GetId() const
{
    return GameId::IW4;
}

const std::string& Game::GetFullName() const
{
    static std::string fullName = "Call Of Duty: Modern Warfare 2";
    return fullName;
}

const std::string& Game::GetShortName() const
{
    static std::string shortName = "IW4";
    return shortName;
}

const std::vector<GameLanguagePrefix>& Game::GetLanguagePrefixes() const
{
    static std::vector<GameLanguagePrefix> prefixes;
    return prefixes;
}
