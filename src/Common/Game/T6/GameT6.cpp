#include "GameT6.h"

#include "T6.h"

#include <algorithm>

using namespace T6;

GameT6 g_GameT6;

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
    const auto foundEntry = std::find(m_zones.begin(), m_zones.end(), zone);

    if (foundEntry != m_zones.end())
        m_zones.erase(foundEntry);
}

std::vector<Zone*> GameT6::GetZones()
{
    return m_zones;
}

std::vector<GameLanguagePrefix> GameT6::GetLanguagePrefixes()
{
    std::vector<GameLanguagePrefix> prefixes;

    prefixes.emplace_back(GameLanguage::LANGUAGE_ENGLISH, "en_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_FRENCH, "fr_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_FRENCH_CAN, "fc_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_GERMAN, "ge_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_AUSTRIAN, "as_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_ITALIAN, "it_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_SPANISH, "sp_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_BRITISH, "br_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_RUSSIAN, "ru_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_POLISH, "po_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_KOREAN, "ko_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_JAPANESE, "ja_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_CZECH, "cz_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_FULL_JAPANESE, "fj_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_PORTUGUESE, "bp_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_MEXICAN_SPANISH, "ms_");

    return prefixes;
}
