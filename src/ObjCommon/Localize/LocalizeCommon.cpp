#include "LocalizeCommon.h"

std::string LocalizeCommon::GetNameOfLanguage(GameLanguage language)
{
    switch (language)
    {
    case GameLanguage::LANGUAGE_NONE:
    case GameLanguage::LANGUAGE_ENGLISH:
    default:
        return "english";

    case GameLanguage::LANGUAGE_FRENCH:
        return "french";

    case GameLanguage::LANGUAGE_GERMAN:
        return "german";

    case GameLanguage::LANGUAGE_ITALIAN:
        return "italian";

    case GameLanguage::LANGUAGE_SPANISH:
        return "spanish";

    case GameLanguage::LANGUAGE_BRITISH:
        return "british";

    case GameLanguage::LANGUAGE_RUSSIAN:
        return "russian";

    case GameLanguage::LANGUAGE_POLISH:
        return "polish";

    case GameLanguage::LANGUAGE_KOREAN:
        return "korean";

    case GameLanguage::LANGUAGE_JAPANESE:
        return "japanese";

    case GameLanguage::LANGUAGE_CZECH:
        return "czech";

    case GameLanguage::LANGUAGE_FRENCH_CAN:
        return "frenchcan";

    case GameLanguage::LANGUAGE_AUSTRIAN:
        return "austrian";

    case GameLanguage::LANGUAGE_PORTUGUESE:
        return "portuguese";

    case GameLanguage::LANGUAGE_MEXICAN_SPANISH:
        return "mexicanspanish";

    case GameLanguage::LANGUAGE_FULL_JAPANESE:
        return "fulljapanese";

    case GameLanguage::LANGUAGE_TAIWANESE:
        return "taiwanese";

    case GameLanguage::LANGUAGE_CHINESE:
        return "chinese";

    case GameLanguage::LANGUAGE_THAI:
        return "thai";

    case GameLanguage::LANGUAGE_LEET:
        return "leet";
    }
}
