#include "GameT5.h"

#include "T5.h"

#include <algorithm>

using namespace T5;

GameT5 g_GameT5;

GameId GameT5::GetId()
{
    return GameId::T5;
}

std::string GameT5::GetFullName()
{
    return "Call Of Duty: Black Ops";
}

std::string GameT5::GetShortName()
{
    return "T5";
}

void GameT5::AddZone(Zone* zone)
{
    m_zones.push_back(zone);
}

void GameT5::RemoveZone(Zone* zone)
{
    const auto foundEntry = std::ranges::find(m_zones, zone);

    if (foundEntry != m_zones.end())
        m_zones.erase(foundEntry);
}

std::vector<Zone*> GameT5::GetZones()
{
    return m_zones;
}

const std::vector<GameLanguagePrefix>& GameT5::GetLanguagePrefixes()
{
    static std::vector<GameLanguagePrefix> prefixes{
        {GameLanguage::LANGUAGE_ENGLISH,    "en_"},
        {GameLanguage::LANGUAGE_FRENCH,     "fr_"},
        {GameLanguage::LANGUAGE_FRENCH_CAN, "fc_"},
        {GameLanguage::LANGUAGE_GERMAN,     "ge_"},
        {GameLanguage::LANGUAGE_AUSTRIAN,   "ge_"},
        {GameLanguage::LANGUAGE_ITALIAN,    "it_"},
        {GameLanguage::LANGUAGE_SPANISH,    "sp_"},
        {GameLanguage::LANGUAGE_BRITISH,    "br_"},
        {GameLanguage::LANGUAGE_RUSSIAN,    "ru_"},
        {GameLanguage::LANGUAGE_POLISH,     "po_"},
        {GameLanguage::LANGUAGE_KOREAN,     "ko_"},
        {GameLanguage::LANGUAGE_JAPANESE,   "ja_"},
        {GameLanguage::LANGUAGE_CZECH,      "cz_"},
    };

    return prefixes;
}
