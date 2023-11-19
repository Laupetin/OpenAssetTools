#include "AssetDumperFontIcon.h"

#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"

#include <sstream>

using namespace T6;

class AssetDumperFontIconInternal
{
    class KnownAlias
    {
    public:
        std::string m_name;
        int m_hash;

        explicit KnownAlias(std::string aliasName)
            : m_name(std::move(aliasName))
        {
            m_hash = Common::Com_HashString(m_name.c_str(), 0);
        }
    };

    inline static const std::string TYPE_ICON = "icon";
    inline static const std::string ICON_HEADERS[]{"# index", "# type", "# name", "# material", "# size", "# xScale", "# yScale"};

    inline static const std::string TYPE_ALIAS = "alias";
    inline static const std::string ALIAS_HEADERS[]{"# index", "# type", "# alias", "# button"};

    inline static const KnownAlias KNOWN_ALIASES[]{KnownAlias("BUTTON_ADS"),
                                                   KnownAlias("BUTTON_CAC_NEXT"),
                                                   KnownAlias("BUTTON_CAC_PREV"),
                                                   KnownAlias("BUTTON_CANCEL"),
                                                   KnownAlias("BUTTON_CAROUSEL_STICK"),
                                                   KnownAlias("BUTTON_CREATE"),
                                                   KnownAlias("BUTTON_CYCLE_LEFT"),
                                                   KnownAlias("BUTTON_CYCLE_LEFT_ACTIVE"),
                                                   KnownAlias("BUTTON_CYCLE_RIGHT"),
                                                   KnownAlias("BUTTON_CYCLE_RIGHT_ACTIVE"),
                                                   KnownAlias("BUTTON_DELETE"),
                                                   KnownAlias("BUTTON_EDIT"),
                                                   KnownAlias("BUTTON_EMBLEM_BACKWARD"),
                                                   KnownAlias("BUTTON_EMBLEM_FLIP"),
                                                   KnownAlias("BUTTON_EMBLEM_FORWARD"),
                                                   KnownAlias("BUTTON_EMBLEM_FORWARD_BACKWARD"),
                                                   KnownAlias("BUTTON_EMBLEM_MOVE"),
                                                   KnownAlias("BUTTON_EMBLEM_OUTLINE"),
                                                   KnownAlias("BUTTON_EMBLEM_PALETTE_CYCLE"),
                                                   KnownAlias("BUTTON_EMBLEM_PALETTE_NEXT"),
                                                   KnownAlias("BUTTON_EMBLEM_PALETTE_PREV"),
                                                   KnownAlias("BUTTON_EMBLEM_RESET"),
                                                   KnownAlias("BUTTON_EMBLEM_ROTATE_LEFT"),
                                                   KnownAlias("BUTTON_EMBLEM_ROTATE_RIGHT"),
                                                   KnownAlias("BUTTON_EMBLEM_SCALE"),
                                                   KnownAlias("BUTTON_FIRE"),
                                                   KnownAlias("BUTTON_FRIENDSLIST"),
                                                   KnownAlias("BUTTON_INTERACT"),
                                                   KnownAlias("BUTTON_LOOKSTICK"),
                                                   KnownAlias("BUTTON_LOOK"),
                                                   KnownAlias("BUTTON_LUI_ALT1"),
                                                   KnownAlias("BUTTON_LUI_ALT2"),
                                                   KnownAlias("BUTTON_LUI_DPAD_ALL"),
                                                   KnownAlias("BUTTON_LUI_DPAD_D"),
                                                   KnownAlias("BUTTON_LUI_DPAD_L"),
                                                   KnownAlias("BUTTON_LUI_DPAD_RL"),
                                                   KnownAlias("BUTTON_LUI_DPAD_R"),
                                                   KnownAlias("BUTTON_LUI_DPAD_UD"),
                                                   KnownAlias("BUTTON_LUI_DPAD_U"),
                                                   KnownAlias("BUTTON_LUI_LEFT_STICK_UP"),
                                                   KnownAlias("BUTTON_LUI_LEFT_TRIGGER"),
                                                   KnownAlias("BUTTON_LUI_PRIMARY"),
                                                   KnownAlias("BUTTON_LUI_RIGHT_STICK"),
                                                   KnownAlias("BUTTON_LUI_RIGHT_TRIGGER"),
                                                   KnownAlias("BUTTON_LUI_SECONDARY"),
                                                   KnownAlias("BUTTON_LUI_SELECT"),
                                                   KnownAlias("BUTTON_LUI_SHOULDERL"),
                                                   KnownAlias("BUTTON_LUI_SHOULDERR"),
                                                   KnownAlias("BUTTON_LUI_START"),
                                                   KnownAlias("BUTTON_MOUSE_CLICK"),
                                                   KnownAlias("BUTTON_MOUSE_CLICK_ACTIVE"),
                                                   KnownAlias("BUTTON_MOUSE_EDIT"),
                                                   KnownAlias("BUTTON_MOUSE_EDIT_ACTIVE"),
                                                   KnownAlias("BUTTON_MOUSE_LEFT"),
                                                   KnownAlias("BUTTON_MOUSE_MIDDLE"),
                                                   KnownAlias("BUTTON_MOUSE_RIGHT"),
                                                   KnownAlias("BUTTON_MOVESTICK"),
                                                   KnownAlias("BUTTON_MOVE"),
                                                   KnownAlias("BUTTON_MP_CANCELCOMMAND"),
                                                   KnownAlias("BUTTON_MP_CHANGESETTINGS"),
                                                   KnownAlias("BUTTON_MP_GAMERCARD"),
                                                   KnownAlias("BUTTON_MP_GAMERREVIEW"),
                                                   KnownAlias("BUTTON_MP_JOINGAME"),
                                                   KnownAlias("BUTTON_MP_KICKPLAYER"),
                                                   KnownAlias("BUTTON_MP_LEAVEGAME"),
                                                   KnownAlias("BUTTON_MP_LOBBY_GAMERCARD"),
                                                   KnownAlias("BUTTON_MP_NOTREADY"),
                                                   KnownAlias("BUTTON_MP_PGDOWN"),
                                                   KnownAlias("BUTTON_MP_PGUP"),
                                                   KnownAlias("BUTTON_MP_READY"),
                                                   KnownAlias("BUTTON_MP_REFRESH"),
                                                   KnownAlias("BUTTON_MP_SCOREBOARD"),
                                                   KnownAlias("BUTTON_MP_SIGNIN"),
                                                   KnownAlias("BUTTON_MP_SPECNEXT"),
                                                   KnownAlias("BUTTON_MP_SPECPREV"),
                                                   KnownAlias("BUTTON_MP_STARTGAME"),
                                                   KnownAlias("BUTTON_MP_TOGGLECHASECAM"),
                                                   KnownAlias("BUTTON_MP_TOGGLEVIEW"),
                                                   KnownAlias("BUTTON_NO"),
                                                   KnownAlias("BUTTON_RECORD_VIEW_NEXT"),
                                                   KnownAlias("BUTTON_RECORD_VIEW_PREV"),
                                                   KnownAlias("BUTTON_SELECTCHOICE"),
                                                   KnownAlias("BUTTON_SP_TOGGLEMENU"),
                                                   KnownAlias("BUTTON_YES"),
                                                   KnownAlias("CP"),
                                                   KnownAlias("FONT_CAPITAL_I"),
                                                   KnownAlias("FONT_NUMBER_ZERO"),
                                                   KnownAlias("KEY_DOWN_ARROW"),
                                                   KnownAlias("KEY_LEFT_ARROW"),
                                                   KnownAlias("KEY_RIGHT_ARROW"),
                                                   KnownAlias("KEY_UP_ARROW"),
                                                   KnownAlias("MOUSE_WHEEL_DOWN"),
                                                   KnownAlias("MOUSE_WHEEL_UP"),
                                                   KnownAlias("Remote_LStick")};

    CsvOutputStream m_csv;

    static FontIconEntry* FindEntryByHash(FontIcon* fontIcon, const int hash)
    {
        int lowerBound = 0;
        int upperBound = fontIcon->numEntries;

        while (true)
        {
            const int entryIndex = (lowerBound + upperBound) / 2;
            auto* entry = &fontIcon->fontIconEntry[entryIndex];

            if (entry->fontIconName.hash == hash)
                return entry;

            if (lowerBound == upperBound)
                return nullptr;

            if (entry->fontIconName.hash < hash)
                lowerBound = entryIndex + 1;
            else
                upperBound = entryIndex - 1;
        }
    }

    static const KnownAlias* FindKnownAliasByHash(const int hash)
    {
        for (const auto& i : KNOWN_ALIASES)
        {
            if (i.m_hash == hash)
                return &i;
        }

        return nullptr;
    }

    void WriteFontIconEntries(FontIcon* fontIcon)
    {
        for (const auto& header : ICON_HEADERS)
            m_csv.WriteColumn(header);
        m_csv.NextRow();

        for (int iconIndex = 0; iconIndex < fontIcon->numEntries; iconIndex++)
        {
            auto* entry = &fontIcon->fontIconEntry[iconIndex];
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

    void WriteFontIconAliases(FontIcon* fontIcon)
    {
        for (const auto& header : ALIAS_HEADERS)
            m_csv.WriteColumn(header);
        m_csv.NextRow();

        for (int aliasIndex = 0; aliasIndex < fontIcon->numAliasEntries; aliasIndex++)
        {
            auto* alias = &fontIcon->fontIconAlias[aliasIndex];
            m_csv.WriteColumn(std::to_string(aliasIndex));
            m_csv.WriteColumn(TYPE_ALIAS);

            auto* knownAlias = FindKnownAliasByHash(alias->aliasHash);
            if (knownAlias)
                m_csv.WriteColumn(knownAlias->m_name);
            else
            {
                std::ostringstream str;
                str << '@' << std::hex << alias->aliasHash;

                m_csv.WriteColumn(str.str());
            }

            auto* referencedEntry = FindEntryByHash(fontIcon, alias->buttonHash);
            if (referencedEntry && referencedEntry->fontIconName.string)
                m_csv.WriteColumn(referencedEntry->fontIconName.string);
            else
                m_csv.WriteColumn("");

            m_csv.NextRow();
        }
    }

public:
    explicit AssetDumperFontIconInternal(std::ostream& stream)
        : m_csv(stream)
    {
    }

    void DumpFontIcon(FontIcon* fontIcon)
    {
        WriteFontIconEntries(fontIcon);
        m_csv.NextRow();

        WriteFontIconAliases(fontIcon);
    }
};

bool AssetDumperFontIcon::ShouldDump(XAssetInfo<FontIcon>* asset)
{
    return true;
}

void AssetDumperFontIcon::DumpAsset(AssetDumpingContext& context, XAssetInfo<FontIcon>* asset)
{
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    AssetDumperFontIconInternal dumper(*assetFile);
    dumper.DumpFontIcon(asset->Asset());
}
