#pragma once
#include "Game/GameLanguage.h"

#include <map>
#include <unordered_set>

class LocalizeFileParserState
{
public:
    bool m_end;

    std::map<std::string, std::string> m_entries;

    GameLanguage m_language;
    std::string m_language_name_caps;

    std::string m_current_reference;
    std::unordered_set<std::string> m_current_reference_languages;

    explicit LocalizeFileParserState(GameLanguage language);
};
