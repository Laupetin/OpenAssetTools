#include "AssetDumperLocalizeEntry.h"

#include <sstream>

#include "Localize/LocalizeCommon.h"
#include "Dumping/Localize/StringFileDumper.h"

using namespace IW3;

void AssetDumperLocalizeEntry::DumpPool(AssetDumpingContext& context, AssetPool<LocalizeEntry>* pool)
{
    if (pool->m_asset_lookup.empty())
        return;

    const auto language = LocalizeCommon::GetNameOfLanguage(context.m_zone->m_language);

    std::ostringstream ss;
    ss << language << "/localizedstrings/" << context.m_zone->m_name << ".str";
    const auto assetFile = context.OpenAssetFile(ss.str());

    if (assetFile)
    {
        StringFileDumper stringFileDumper(context.m_zone, *assetFile);

        stringFileDumper.SetLanguageName(language);

        // Magic string. Original string files do have this config file. The purpose of the config file is unknown though.
        stringFileDumper.SetConfigFile(R"(C:\trees\cod3\cod3\bin\StringEd.cfg)");

        stringFileDumper.SetNotes("");

        for (auto* localizeEntry : *pool)
        {
            stringFileDumper.WriteLocalizeEntry(localizeEntry->m_name, localizeEntry->Asset()->value);
        }

        stringFileDumper.Finalize();
    }
    else
    {
        printf("Could not create string file for dumping localized strings of zone '%s'\n", context.m_zone->m_name.c_str());
    }
}