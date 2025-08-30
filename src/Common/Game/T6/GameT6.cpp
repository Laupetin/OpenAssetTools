#include "GameT6.h"

#include <algorithm>

using namespace T6;

GameId Game::GetId() const
{
    return GameId::T6;
}

const std::string& Game::GetFullName() const
{
    static std::string fullName = "Call Of Duty: Black Ops II";
    return fullName;
}

const std::string& Game::GetShortName() const
{
    static std::string shortName = "T6";
    return shortName;
}

const std::vector<GameLanguagePrefix>& Game::GetLanguagePrefixes() const
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
