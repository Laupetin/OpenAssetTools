#include "AssetDumperLocalizeEntry.h"
#include "Dumping/Localize/StringFileDumper.h"

using namespace T6;

std::string AssetDumperLocalizeEntry::GetNameOfLanguage(GameLanguage language)
{
    switch(language)
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
    }
}

void AssetDumperLocalizeEntry::DumpPool(Zone* zone, AssetPool<LocalizeEntry>* pool, const std::string& basePath)
{
    if (pool->m_asset_lookup.empty())
        return;

    const std::string language = GetNameOfLanguage(zone->m_language);
    const std::string stringsPath = utils::Path::Combine(basePath, language + "/localizedstrings");

    FileAPI::DirectoryCreate(stringsPath);

    FileAPI::File stringFile = FileAPI::Open(utils::Path::Combine(stringsPath, zone->m_name + ".str"), FileAPI::Mode::MODE_WRITE);

    if(stringFile.IsOpen())
    {
        StringFileDumper stringFileDumper(zone, &stringFile);

        stringFileDumper.SetLanguageName(language);

        // Magic string. Original string files do have this config file. The purpose of the config file is unknown though.
        stringFileDumper.SetConfigFile(R"(C:\projects\cod\t6\bin\StringEd.cfg)");

        stringFileDumper.SetNotes("");

        for(auto localizeEntry : *pool)
        {
            stringFileDumper.WriteLocalizeEntry(localizeEntry->m_name, localizeEntry->Asset()->value);
        }

        stringFileDumper.Finalize();

        stringFile.Close();
    }
    else
    {
        printf("Could not create string file for dumping localized strings of zone '%s'\n", zone->m_name.c_str());
    }
}