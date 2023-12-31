#pragma once

#include "Game/GameLanguage.h"
#include "Localize/LocalizeReadingZoneState.h"

#include <map>
#include <unordered_set>

class LocalizeFileParserState
{
public:
    bool m_end;

    std::map<std::string, std::string> m_entries;

    GameLanguage m_language;
    LocalizeReadingZoneState* m_zone_state;
    std::string m_language_name_caps;

    std::string m_current_reference;
    std::unordered_set<std::string> m_current_reference_languages;

    LocalizeFileParserState(GameLanguage language, LocalizeReadingZoneState* zoneState);
};
