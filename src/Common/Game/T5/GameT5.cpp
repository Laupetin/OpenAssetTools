#include "GameT5.h"

#include <algorithm>

using namespace T5;

GameId Game::GetId() const
{
    return GameId::T5;
}

const std::string& Game::GetFullName() const
{
    static std::string fullName = "Call Of Duty: Black Ops";
    return fullName;
}

const std::string& Game::GetShortName() const
{
    static std::string shortName = "T5";
    return shortName;
}

const std::vector<GameLanguagePrefix>& Game::GetLanguagePrefixes() const
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
