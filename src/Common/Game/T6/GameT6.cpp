#include "GameT6.h"

#include "T6.h"

#include <algorithm>

using namespace T6;

GameT6 g_GameT6;

GameId GameT6::GetId()
{
    return GameId::T6;
}

std::string GameT6::GetFullName()
{
    return "Call Of Duty: Black Ops II";
}

std::string GameT6::GetShortName()
{
    return "T6";
}

void GameT6::AddZone(Zone* zone)
{
    m_zones.push_back(zone);
}

void GameT6::RemoveZone(Zone* zone)
{
    const auto foundEntry = std::ranges::find(m_zones, zone);

    if (foundEntry != m_zones.end())
        m_zones.erase(foundEntry);
}

std::vector<Zone*> GameT6::GetZones()
{
    return m_zones;
}

const std::vector<GameLanguagePrefix>& GameT6::GetLanguagePrefixes()
{
    static std::vector<GameLanguagePrefix> prefixes{
        {GameLanguage::LANGUAGE_ENGLISH,         "en_"},
        {GameLanguage::LANGUAGE_FRENCH,          "fr_"},
        {GameLanguage::LANGUAGE_FRENCH_CAN,      "fc_"},
        {GameLanguage::LANGUAGE_GERMAN,          "ge_"},
        {GameLanguage::LANGUAGE_AUSTRIAN,        "as_"},
        {GameLanguage::LANGUAGE_ITALIAN,         "it_"},
        {GameLanguage::LANGUAGE_SPANISH,         "sp_"},
        {GameLanguage::LANGUAGE_BRITISH,         "br_"},
        {GameLanguage::LANGUAGE_RUSSIAN,         "ru_"},
        {GameLanguage::LANGUAGE_POLISH,          "po_"},
        {GameLanguage::LANGUAGE_KOREAN,          "ko_"},
        {GameLanguage::LANGUAGE_JAPANESE,        "ja_"},
        {GameLanguage::LANGUAGE_CZECH,           "cz_"},
        {GameLanguage::LANGUAGE_FULL_JAPANESE,   "fj_"},
        {GameLanguage::LANGUAGE_PORTUGUESE,      "bp_"},
        {GameLanguage::LANGUAGE_MEXICAN_SPANISH, "ms_"},
    };

    return prefixes;
}
