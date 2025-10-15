#include "FontIconCsvDumperT6.h"

#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"
#include "KnownFontIconAliasesT6.h"

#include <format>

using namespace T6;

namespace
{
    const std::string TYPE_ICON = "icon";
    const std::string ICON_HEADERS[]{
        "# index",
        "# type",
        "# name",
        "# material",
        "# size",
        "# xScale",
        "# yScale",
    };

    const std::string TYPE_ALIAS = "alias";
    const std::string ALIAS_HEADERS[]{
        "# index",
        "# type",
        "# alias",
        "# button",
    };

    FontIconEntry* FindEntryByHash(const FontIcon& fontIcon, const int hash)
    {
        auto lowerBound = 0u;
        auto upperBound = fontIcon.numEntries;

        while (true)
        {
            const auto entryIndex = (lowerBound + upperBound) / 2u;
            auto* entry = &fontIcon.fontIconEntry[entryIndex];

            if (entry->fontIconName.hash == hash)
                return entry;

            if (lowerBound == upperBound)
                return nullptr;

            if (entry->fontIconName.hash < hash)
                lowerBound = entryIndex + 1u;
            else
                upperBound = entryIndex - 1u;
        }
    }

    class Dumper
    {
    public:
        explicit Dumper(std::ostream& stream)
            : m_csv(stream)
        {
        }

        void Dump(const FontIcon& fontIcon)
        {
            WriteFontIconEntries(fontIcon);
            m_csv.NextRow();

            WriteFontIconAliases(fontIcon);
        }

    private:
        void WriteFontIconEntries(const FontIcon& fontIcon)
        {
            for (const auto& header : ICON_HEADERS)
                m_csv.WriteColumn(header);
            m_csv.NextRow();

            for (auto iconIndex = 0u; iconIndex < fontIcon.numEntries; iconIndex++)
            {
                const auto* entry = &fontIcon.fontIconEntry[iconIndex];
                m_csv.WriteColumn(std::to_string(iconIndex));
                m_csv.WriteColumn(TYPE_ICON);

                if (entry->fontIconName.string)
                    m_csv.WriteColumn(entry->fontIconName.string);
                else
                    m_csv.WriteColumn("");

                if (entry->fontIconMaterialHandle && entry->fontIconMaterialHandle->info.name)
                    m_csv.WriteColumn(entry->fontIconMaterialHandle->info.name);
                else
                    m_csv.WriteColumn("");

                m_csv.WriteColumn(std::to_string(entry->fontIconSize));
                m_csv.WriteColumn(std::to_string(entry->xScale));
                m_csv.WriteColumn(std::to_string(entry->yScale));

                m_csv.NextRow();
            }
        }

        void WriteFontIconAliases(const FontIcon& fontIcon)
        {
            for (const auto& header : ALIAS_HEADERS)
                m_csv.WriteColumn(header);
            m_csv.NextRow();

            for (auto aliasIndex = 0u; aliasIndex < fontIcon.numAliasEntries; aliasIndex++)
            {
                const auto* alias = &fontIcon.fontIconAlias[aliasIndex];
                m_csv.WriteColumn(std::to_string(aliasIndex));
                m_csv.WriteColumn(TYPE_ALIAS);

                auto* knownAlias = FindKnownFontIconAliasByHash(alias->aliasHash);
                if (knownAlias)
                    m_csv.WriteColumn(knownAlias->m_name);
                else
                    m_csv.WriteColumn(std::format("@{:x}", alias->aliasHash));

                const auto* referencedEntry = FindEntryByHash(fontIcon, alias->buttonHash);
                if (referencedEntry && referencedEntry->fontIconName.string)
                    m_csv.WriteColumn(referencedEntry->fontIconName.string);
                else
                    m_csv.WriteColumn("");

                m_csv.NextRow();
            }
        }

        CsvOutputStream m_csv;
    };
} // namespace

namespace font_icon
{
    CsvDumperT6::CsvDumperT6(const AssetPool<AssetFontIcon::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void CsvDumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetFontIcon::Type>& asset)
    {
        const auto assetFile = context.OpenAssetFile(asset.m_name);

        if (!assetFile)
            return;

        Dumper dumper(*assetFile);
        dumper.Dump(*asset.Asset());
    }
} // namespace font_icon
