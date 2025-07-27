#include "FontIconJsonDumperT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/FontIcon/FontIconCommonT6.h"
#include "Game/T6/FontIcon/JsonFontIconT6.h"
#include "KnownFontIconAliasesT6.h"

#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;

namespace
{
    void AddFontIconEntryAlias(JsonFontIconEntry& jFontIconEntry, const int aliasHash)
    {
        auto* knownAlias = FindKnownFontIconAliasByHash(aliasHash);
        if (knownAlias)
        {
            jFontIconEntry.aliases.emplace_back(knownAlias->m_name);
        }
        else
        {
            if (!jFontIconEntry.aliasHashes.has_value())
                jFontIconEntry.aliasHashes.emplace();

            jFontIconEntry.aliasHashes->emplace_back(static_cast<unsigned>(aliasHash));
        }
    }

    void CreateJsonFontIconEntry(JsonFontIconEntry& jFontIconEntry, const FontIconEntry& fontIconEntry, const FontIcon& fontIcon)
    {
        jFontIconEntry.name = fontIconEntry.fontIconName.string;

        if (fontIconEntry.fontIconMaterialHandle && fontIconEntry.fontIconMaterialHandle->info.name)
            jFontIconEntry.material = fontIconEntry.fontIconMaterialHandle->info.name;

        jFontIconEntry.size = static_cast<unsigned>(fontIconEntry.fontIconSize);
        jFontIconEntry.scale = JsonVec2();
        jFontIconEntry.scale->x = fontIconEntry.xScale;
        jFontIconEntry.scale->y = fontIconEntry.yScale;

        if (fontIcon.fontIconAlias)
        {
            for (auto aliasIndex = 0u; aliasIndex < fontIcon.numAliasEntries; aliasIndex++)
            {
                const auto& alias = fontIcon.fontIconAlias[aliasIndex];
                if (alias.buttonHash == fontIconEntry.fontIconName.hash)
                    AddFontIconEntryAlias(jFontIconEntry, alias.aliasHash);
            }
        }
    }

    void CreateJsonFontIconFile(JsonFontIcon& jFontIcon, const FontIcon& fontIcon)
    {
        if (fontIcon.fontIconEntry == nullptr)
            return;

        jFontIcon.entries.resize(fontIcon.numEntries);
        for (auto i = 0u; i < fontIcon.numEntries; i++)
            CreateJsonFontIconEntry(jFontIcon.entries[i], fontIcon.fontIconEntry[i], fontIcon);
    }

    void DumpFontIcon(std::ostream& stream, const FontIcon& fontIcon)
    {
        JsonFontIcon jFontIcon;
        CreateJsonFontIconFile(jFontIcon, fontIcon);
        json jRoot = jFontIcon;

        jRoot["$schema"] = "http://openassettools.dev/schema/font-icon.v1.json";
        jRoot["_type"] = "font-icon";
        jRoot["_version"] = 1;
        jRoot["_game"] = "t6";

        stream << std::setw(4) << jRoot << "\n";
    }
} // namespace

namespace T6::font_icon
{
    bool JsonDumper::ShouldDump(XAssetInfo<FontIcon>* asset)
    {
        return true;
    }

    void JsonDumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<FontIcon>* asset)
    {
        const auto assetFile = context.OpenAssetFile(font_icon::GetJsonFileNameForAssetName(asset->m_name));

        if (!assetFile)
            return;

        DumpFontIcon(*assetFile, *asset->Asset());
    }
} // namespace T6::font_icon
