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

std::vector<GameLanguagePrefix> GameT5::GetLanguagePrefixes()
{
    std::vector<GameLanguagePrefix> prefixes;

    prefixes.emplace_back(GameLanguage::LANGUAGE_ENGLISH, "en_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_FRENCH, "fr_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_FRENCH_CAN, "fc_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_GERMAN, "ge_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_AUSTRIAN, "ge_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_ITALIAN, "it_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_SPANISH, "sp_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_BRITISH, "br_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_RUSSIAN, "ru_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_POLISH, "po_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_KOREAN, "ko_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_JAPANESE, "ja_");
    prefixes.emplace_back(GameLanguage::LANGUAGE_CZECH, "cz_");

    return prefixes;
}
