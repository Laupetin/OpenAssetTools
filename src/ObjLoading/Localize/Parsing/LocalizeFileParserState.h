#pragma once

#include "Game/GameLanguage.h"
#include "ILocalizeFileDuplicationChecker.h"
#include "Localize/CommonLocalizeEntry.h"

#include <unordered_set>
#include <vector>

class LocalizeFileParserState
{
public:
    LocalizeFileParserState(GameLanguage language, ILocalizeFileDuplicationChecker& duplicationChecker);

    bool m_end;

    std::vector<CommonLocalizeEntry> m_entries;

    GameLanguage m_language;
    ILocalizeFileDuplicationChecker& m_duplication_checker;
    std::string m_language_name_caps;

    std::string m_current_reference;
    std::unordered_set<std::string> m_current_reference_languages;
};
