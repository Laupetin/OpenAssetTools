#pragma once
#include "Game/GameLanguage.h"
#include "Localize/LocalizeFile.h"

#include <unordered_set>
#include <vector>

class LocalizeFileParserState
{
public:
    bool m_end;

    std::vector<LocalizeFileEntry> m_entries;

    GameLanguage m_language;
    std::string m_language_name_caps;

    std::string m_current_reference;
    std::unordered_set<std::string> m_current_reference_languages;

    explicit LocalizeFileParserState(GameLanguage language);
};
