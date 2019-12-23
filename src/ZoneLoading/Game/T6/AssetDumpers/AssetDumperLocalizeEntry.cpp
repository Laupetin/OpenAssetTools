#include "AssetDumperLocalizeEntry.h"
#include "Dumping/Localize/StringFileDumper.h"

using namespace T6;

std::string AssetDumperLocalizeEntry::GetNameOfLanguage(ZoneLanguage language)
{
    switch(language)
    {
    case ZoneLanguage::LANGUAGE_NONE:
    case ZoneLanguage::LANGUAGE_ENGLISH:
    default:
        return "english";

    case ZoneLanguage::LANGUAGE_FRENCH:
        return "french";

    case ZoneLanguage::LANGUAGE_GERMAN:
        return "german";

    case ZoneLanguage::LANGUAGE_ITALIAN:
        return "italian";

    case ZoneLanguage::LANGUAGE_SPANISH:
        return "spanish";

    case ZoneLanguage::LANGUAGE_BRITISH:
        return "british";

    case ZoneLanguage::LANGUAGE_RUSSIAN:
        return "russian";

    case ZoneLanguage::LANGUAGE_POLISH:
        return "polish";

    case ZoneLanguage::LANGUAGE_KOREAN:
        return "korean";

    case ZoneLanguage::LANGUAGE_JAPANESE:
        return "japanese";

    case ZoneLanguage::LANGUAGE_CZECH:
        return "czech";

    case ZoneLanguage::LANGUAGE_FRENCH_CAN:
        return "frenchcan";

    case ZoneLanguage::LANGUAGE_AUSTRIAN:
        return "austrian";

    case ZoneLanguage::LANGUAGE_PORTUGUESE:
        return "portuguese";

    case ZoneLanguage::LANGUAGE_MEXICAN_SPANISH:
        return "mexicanspanish";

    case ZoneLanguage::LANGUAGE_FULL_JAPANESE:
        return "fulljapanese";
    }
}

void AssetDumperLocalizeEntry::DumpPool(Zone* zone, AssetPool<LocalizeEntry>* pool, const std::string& basePath)
{
    if (pool->m_asset_lookup.size() == 0)
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
            stringFileDumper.WriteLocalizeEntry(localizeEntry->m_asset->name, localizeEntry->m_asset->value);
        }

        stringFileDumper.Finalize();

        stringFile.Close();
    }
    else
    {
        printf("Could not create string file for dumping localized strings of zone '%s'\n", zone->m_name.c_str());
    }
}