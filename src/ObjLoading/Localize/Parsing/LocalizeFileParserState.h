#pragma once

#include "Game/GameLanguage.h"
#include "Localize/CommonLocalizeEntry.h"
#include "Localize/LocalizeReadingZoneState.h"

#include <unordered_set>
#include <vector>

class LocalizeFileParserState
{
public:
    bool m_end;

    std::vector<CommonLocalizeEntry> m_entries;

    GameLanguage m_language;
    LocalizeReadingZoneState* m_zone_state;
    std::string m_language_name_caps;

    std::string m_current_reference;
    std::unordered_set<std::string> m_current_reference_languages;

    LocalizeFileParserState(GameLanguage language, LocalizeReadingZoneState* zoneState);
};
