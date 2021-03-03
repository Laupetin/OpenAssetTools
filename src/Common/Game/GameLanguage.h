#pragma once

#include <string>

enum class GameLanguage
{
    LANGUAGE_NONE,
    LANGUAGE_ENGLISH,
    LANGUAGE_FRENCH,
    LANGUAGE_GERMAN,
    LANGUAGE_ITALIAN,
    LANGUAGE_SPANISH,
    LANGUAGE_BRITISH,
    LANGUAGE_RUSSIAN,
    LANGUAGE_POLISH,
    LANGUAGE_KOREAN,
    LANGUAGE_TAIWANESE,
    LANGUAGE_JAPANESE,
    LANGUAGE_CHINESE,
    LANGUAGE_THAI,
    LANGUAGE_LEET,
    LANGUAGE_CZECH,
    LANGUAGE_FRENCH_CAN,
    LANGUAGE_AUSTRIAN,
    LANGUAGE_PORTUGUESE,
    LANGUAGE_MEXICAN_SPANISH,
    LANGUAGE_FULL_JAPANESE
};

class GameLanguagePrefix
{
public:
    GameLanguage m_language;
    const std::string m_prefix;

    GameLanguagePrefix(const GameLanguage language, std::string prefix)
        : m_language(language),
          m_prefix(std::move(prefix))
    {
    }
};
