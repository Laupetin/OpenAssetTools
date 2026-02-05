#include "LocalizeDumperT6.h"

#include "Dumping/Localize/StringFileDumper.h"
#include "Localize/LocalizeCommon.h"
#include "Utils/Logging/Log.h"

#include <format>

using namespace T6;

namespace localize
{
    void DumperT6::Dump(AssetDumpingContext& context)
    {
        auto localizeAssets = context.m_zone.m_pools.PoolAssets<AssetLocalize>();
        if (localizeAssets.empty())
            return;

        const auto language = LocalizeCommon::GetNameOfLanguage(context.m_zone.m_language);
        const auto assetFile = context.OpenAssetFile(std::format("{}/localizedstrings/{}.str", language, context.m_zone.m_name));

        if (assetFile)
        {
            StringFileDumper stringFileDumper(context.m_zone, *assetFile);

            stringFileDumper.SetLanguageName(language);

            // Magic string. Original string files do have this config file. The purpose of the config file is unknown though.
            stringFileDumper.SetConfigFile(R"(C:/projects/cod/t6/bin/StringEd.cfg)");

            stringFileDumper.SetNotes("");

            for (const auto* localizeEntry : localizeAssets)
            {
                stringFileDumper.WriteLocalizeEntry(localizeEntry->m_name, localizeEntry->Asset()->value);
            }

            stringFileDumper.Finalize();
        }
        else
        {
            con::error("Could not create string file for dumping localized strings of zone '{}'", context.m_zone.m_name);
        }

        context.IncrementProgress();
    }
} // namespace localize
