#include "AssetDumperLocalizeEntry.h"

#include <fstream>
#include <filesystem>

#include "Localize/LocalizeCommon.h"
#include "Dumping/Localize/StringFileDumper.h"

using namespace IW4;
namespace fs = std::filesystem;

void AssetDumperLocalizeEntry::DumpPool(AssetDumpingContext& context, AssetPool<LocalizeEntry>* pool)
{
    if (pool->m_asset_lookup.empty())
        return;

    const auto language = LocalizeCommon::GetNameOfLanguage(context.m_zone->m_language);
    fs::path stringsPath(context.m_base_path);
    stringsPath.append(language);
    stringsPath.append("/localizedstrings");

    create_directories(stringsPath);

    auto stringFilePath(stringsPath);
    stringFilePath.append(context.m_zone->m_name);
    stringFilePath.append(".str");

    std::ofstream stringFile(stringFilePath, std::fstream::out | std::ofstream::binary);

    if (stringFile.is_open())
    {
        StringFileDumper stringFileDumper(context.m_zone, stringFile);

        stringFileDumper.SetLanguageName(language);

        // Magic string. Original string files do have this config file. The purpose of the config file is unknown though.
        stringFileDumper.SetConfigFile(R"(C:\trees\cod3\cod3\bin\StringEd.cfg)");

        stringFileDumper.SetNotes("");

        for (auto* localizeEntry : *pool)
        {
            stringFileDumper.WriteLocalizeEntry(localizeEntry->m_name, localizeEntry->Asset()->value);
        }

        stringFileDumper.Finalize();

        stringFile.close();
    }
    else
    {
        printf("Could not create string file for dumping localized strings of zone '%s'\n", context.m_zone->m_name.c_str());
    }
}