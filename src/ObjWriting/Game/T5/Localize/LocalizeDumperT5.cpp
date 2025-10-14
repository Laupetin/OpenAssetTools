#include "LocalizeDumperT5.h"

#include "Dumping/Localize/StringFileDumper.h"
#include "Localize/LocalizeCommon.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>

using namespace T5;

namespace localize
{
    size_t DumperT5::GetProgressTotalCount(const AssetPool<T5::LocalizeEntry>& pool) const
    {
        return pool.m_asset_lookup.empty() ? 0uz : 1uz;
    }

    void DumperT5::DumpPool(AssetDumpingContext& context, const AssetPool<LocalizeEntry>& pool)
    {
        if (pool.m_asset_lookup.empty())
            return;

        const auto language = LocalizeCommon::GetNameOfLanguage(context.m_zone.m_language);
        const auto assetFile = context.OpenAssetFile(std::format("{}/localizedstrings/{}.str", language, context.m_zone.m_name));

        if (assetFile)
        {
            StringFileDumper stringFileDumper(context.m_zone, *assetFile);

            stringFileDumper.SetLanguageName(language);

            // Magic string. Original string files do have this config file. The purpose of the config file is unknown though.
            stringFileDumper.SetConfigFile(R"(C:/projects/cod/t5/bin/StringEd.cfg)");

            stringFileDumper.SetNotes("");

            for (const auto* localizeEntry : pool)
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
