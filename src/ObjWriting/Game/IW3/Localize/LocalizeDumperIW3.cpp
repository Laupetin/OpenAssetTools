#include "LocalizeDumperIW3.h"

#include "Dumping/Localize/StringFileDumper.h"
#include "Localize/LocalizeCommon.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>

using namespace IW3;

namespace localize
{
    DumperIW3::DumperIW3(const AssetPool<IW3::AssetLocalize::Type>& pool)
        : AbstractSingleProgressAssetDumper(pool)
    {
    }

    size_t DumperIW3::GetProgressTotalCount() const
    {
        return m_pool.m_asset_lookup.empty() ? 0uz : 1uz;
    }

    void DumperIW3::Dump(AssetDumpingContext& context)
    {
        if (m_pool.m_asset_lookup.empty())
            return;

        const auto language = LocalizeCommon::GetNameOfLanguage(context.m_zone.m_language);
        const auto assetFile = context.OpenAssetFile(std::format("{}/localizedstrings/{}.str", language, context.m_zone.m_name));

        if (assetFile)
        {
            StringFileDumper stringFileDumper(context.m_zone, *assetFile);

            stringFileDumper.SetLanguageName(language);

            // Magic string. Original string files do have this config file. The purpose of the config file is unknown though.
            stringFileDumper.SetConfigFile(R"(C:/trees/cod3/cod3/bin/StringEd.cfg)");

            stringFileDumper.SetNotes("");

            for (const auto* localizeEntry : m_pool)
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
