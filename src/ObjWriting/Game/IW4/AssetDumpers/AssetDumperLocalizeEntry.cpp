#include "AssetDumperLocalizeEntry.h"

#include "Dumping/Localize/LocalizeCommon.h"
#include "Dumping/Localize/StringFileDumper.h"

using namespace IW4;

void AssetDumperLocalizeEntry::DumpPool(Zone* zone, AssetPool<LocalizeEntry>* pool, const std::string& basePath)
{
    if (pool->m_asset_lookup.empty())
        return;

    const std::string language = LocalizeCommon::GetNameOfLanguage(zone->m_language);
    const std::string stringsPath = utils::Path::Combine(basePath, language + "/localizedstrings");

    FileAPI::DirectoryCreate(stringsPath);

    FileAPI::File stringFile = FileAPI::Open(utils::Path::Combine(stringsPath, zone->m_name + ".str"), FileAPI::Mode::MODE_WRITE);

    if (stringFile.IsOpen())
    {
        StringFileDumper stringFileDumper(zone, &stringFile);

        stringFileDumper.SetLanguageName(language);

        // Magic string. Original string files do have this config file. The purpose of the config file is unknown though.
        stringFileDumper.SetConfigFile(R"(C:\trees\cod3\cod3\bin\StringEd.cfg)");

        stringFileDumper.SetNotes("");

        for (auto localizeEntry : *pool)
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