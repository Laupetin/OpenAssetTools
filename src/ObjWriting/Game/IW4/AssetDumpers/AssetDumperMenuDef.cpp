#include "AssetDumperMenuDef.h"

#include <filesystem>
#include <ostream>
#include <cmath>

#include "Game/IW4/GameAssetPoolIW4.h"
#include "Menu/MenuDumper.h"

namespace fs = std::filesystem;

using namespace IW4;

namespace IW4
{
    const char* g_expFunctionNames[]
    {
        "NOOP",
        ")",
        "*",
        "/",
        "%",
        "+",
        "-",
        "!",
        "<",
        "<=",
        ">",
        ">=",
        "==",
        "!=",
        "&&",
        "||",
        "(",
        ",",
        "&",
        "|",
        "~",
        "<<",
        ">>",
        "dvarint(static)\x01\x02",
        "dvarbool(static)\x01\x03",
        "dvarfloat(static)\x01\x04",
        "dvarstring(static)\x01\x05",
        "int",
        "string",
        "float",
        "sin",
        "cos",
        "min",
        "max",
        "milliseconds",
        "dvarint",
        "dvarbool",
        "dvarfloat",
        "dvarstring",
        "stat",
        "ui_active",
        "flashbanged",
        "usingvehicle",
        "missilecam",
        "scoped",
        "scopedthermal",
        "scoreboard_visible",
        "inkillcam",
        "inkillcamnpc",
        "player",
        "getperk",
        "selecting_location",
        "selecting_direction",
        "team",
        "otherteam",
        "marinesfield",
        "opforfield",
        "menuisopen",
        "writingdata",
        "inlobby",
        "inprivateparty",
        "privatepartyhost",
        "privatepartyhostinlobby",
        "aloneinparty",
        "adsjavelin",
        "weaplockblink",
        "weapattacktop",
        "weapattackdirect",
        "weaplocking",
        "weaplocked",
        "weaplocktooclose",
        "weaplockscreenposx",
        "weaplockscreenposy",
        "secondsastime",
        "tablelookup",
        "tablelookupbyrow",
        "tablegetrownum",
        "locstring",
        "localvarint",
        "localvarbool",
        "localvarfloat",
        "localvarstring",
        "timeleft",
        "secondsascountdown",
        "gamemsgwndactive",
        "gametypename",
        "gametype",
        "gametypedescription",
        "scoreatrank",
        "friendsonline",
        "spectatingclient",
        "spectatingfree",
        "statrangeanybitsset",
        "keybinding",
        "actionslotusable",
        "hudfade",
        "maxrecommendedplayers",
        "acceptinginvite",
        "isintermission",
        "gamehost",
        "partyismissingmappack",
        "partymissingmappackerror",
        "anynewmappacks",
        "amiselected",
        "partystatusstring",
        "attachedcontrollercount",
        "issplitscreenonlinepossible",
        "splitscreenplayercount",
        "getplayerdata",
        "getplayerdatasplitscreen",
        "experienceforlevel",
        "levelforexperience",
        "isitemunlocked",
        "isitemunlockedsplitscreen",
        "debugprint",
        "getplayerdataanybooltrue",
        "weaponclassnew",
        "weaponname",
        "isreloading",
        "savegameavailable",
        "unlockeditemcount",
        "unlockeditemcountsplitscreen",
        "unlockeditem",
        "unlockeditemsplitscreen",
        "mailsubject",
        "mailfrom",
        "mailreceived",
        "mailbody",
        "maillootlocalized",
        "mailgivesloot",
        "anynewmail",
        "mailtimetofollowup",
        "mailloottype",
        "mailranlottery",
        "lotterylootlocalized",
        "radarisjammed",
        "radarjamintensity",
        "radarisenabled",
        "isempjammed",
        "playerads",
        "weaponheatactive",
        "weaponheatvalue",
        "weaponheatoverheated",
        "getsplashtext",
        "getsplashdescription",
        "getsplashmaterial",
        "splashhasicon",
        "splashrownum",
        "getfocuseditemname",
        "getfocuseditemx",
        "getfocuseditemy",
        "getfocuseditemwidth",
        "getfocuseditemheight",
        "getitemx",
        "getitemy",
        "getitemwidth",
        "getitemheight",
        "playlist",
        "scoreboardexternalmutenotice",
        "getclientmatchdata",
        "getclientmatchdatadef",
        "getmapname",
        "getmapimage",
        "getmapcustom",
        "getmigrationstatus",
        "getplayercardinfo",
        "isofflineprofileselected",
        "coopplayer",
        "iscoop",
        "getpartystatus",
        "getsearchparams",
        "gettimeplayed",
        "isselectedplayerfriend",
        "getcharbyindex",
        "getprofiledata",
        "isprofilesignedin",
        "getwaitpopupstatus",
        "getnattype",
        "getlocalizednattype",
        "getadjustedsafeareahorizontal",
        "getadjustedsafeareavertical",
        "connectioninfo",
        "offlineprofilecansave",
        "allsplitscreenprofilescansave",
        "allsplitscreenprofilesaresignedin",
        "coopready",
    };
}

class MenuDumperIw4 : public MenuDumper
{
    static constexpr auto MENU_KEY_SPACING = 28u;
    static const inline std::string BOOL_VALUE_TRUE = "1";
    static const inline std::string BOOL_VALUE_FALSE = "0";
    static constexpr inline float COLOR_0000[4]{0.0f, 0.0f, 0.0f, 0.0f};
    static constexpr inline float COLOR_1111[4]{1.0f, 1.0f, 1.0f, 1.0f};

    static const std::string& BoolValue(const bool value)
    {
        return value ? BOOL_VALUE_TRUE : BOOL_VALUE_FALSE;
    }

    void WriteKey(const std::string& keyName) const
    {
        m_stream << keyName;

        if (keyName.size() < MENU_KEY_SPACING)
        {
            const auto spacingLength = MENU_KEY_SPACING - keyName.size();
            for (auto i = 0u; i < spacingLength; i++)
                m_stream << " ";
        }
    }

    void WriteStringProperty(const std::string& propertyKey, const std::string& propertyValue) const
    {
        if (propertyValue.empty())
            return;

        Indent();
        WriteKey(propertyKey);
        m_stream << "\"" << propertyValue << "\"\n";
    }

    void WriteStringProperty(const std::string& propertyKey, const char* propertyValue) const
    {
        if (propertyValue == nullptr || propertyValue[0] == '\0')
            return;

        Indent();
        WriteKey(propertyKey);
        m_stream << "\"" << propertyValue << "\"\n";
    }

    void WriteBoolProperty(const std::string& propertyKey, const bool propertyValue, const bool defaultValue) const
    {
        if (propertyValue == defaultValue)
            return;

        Indent();
        WriteKey(propertyKey);
        m_stream << BoolValue(propertyValue) << "\n";
    }

    void WriteIntProperty(const std::string& propertyKey, const int propertyValue, const int defaultValue) const
    {
        if (propertyValue == defaultValue)
            return;

        Indent();
        WriteKey(propertyKey);
        m_stream << propertyValue << "\n";
    }

    void WriteFloatProperty(const std::string& propertyKey, const float propertyValue, const float defaultValue) const
    {
        if (std::fabs(propertyValue - defaultValue) < std::numeric_limits<float>::epsilon())
            return;

        Indent();
        WriteKey(propertyKey);
        m_stream << propertyValue << "\n";
    }

    void WriteColorProperty(const std::string& propertyKey, const float (&propertyValue)[4], const float (&defaultValue)[4]) const
    {
        if (std::fabs(propertyValue[0] - defaultValue[0]) < std::numeric_limits<float>::epsilon()
            && std::fabs(propertyValue[1] - defaultValue[1]) < std::numeric_limits<float>::epsilon()
            && std::fabs(propertyValue[2] - defaultValue[2]) < std::numeric_limits<float>::epsilon()
            && std::fabs(propertyValue[3] - defaultValue[3]) < std::numeric_limits<float>::epsilon())
        {
            return;
        }

        Indent();
        WriteKey(propertyKey);
        m_stream << propertyValue[0] << " " << propertyValue[1] << " " << propertyValue[2] << " " << propertyValue[3] << "\n";
    }

    void WriteKeywordProperty(const std::string& propertyKey, const bool shouldWrite) const
    {
        if (!shouldWrite)
            return;

        Indent();
        WriteKey(propertyKey);
        m_stream << "\n";
    }

    void WriteFlagsProperty(const std::string& propertyKey, const int flagsValue) const
    {
        for (auto i = 0u; i < sizeof(flagsValue) * 8; i++)
        {
            if (flagsValue & (1 << i))
            {
                Indent();
                WriteKey(propertyKey);
                m_stream << i << "\n";
            }
        }
    }

    void WriteRectProperty(const std::string& propertyKey, const rectDef_s& rect) const
    {
        Indent();
        WriteKey(propertyKey);
        m_stream << rect.x << " " << rect.y << " " << rect.w << " " << rect.h << " " << static_cast<int>(rect.horzAlign) << " " << static_cast<int>(rect.vertAlign) << "\n";
    }

    void WriteMaterialProperty(const std::string& propertyKey, const Material* materialValue) const
    {
        if (materialValue == nullptr)
            return;

        WriteStringProperty(propertyKey, materialValue->info.name);
    }

    void WriteSoundAliasProperty(const std::string& propertyKey, const snd_alias_list_t* soundAliasValue) const
    {
        if (soundAliasValue == nullptr)
            return;

        WriteStringProperty(propertyKey, soundAliasValue->aliasName);
    }

    void WriteStatementProperty(const std::string& propertyKey, const Statement_s* statementValue, const bool isBooleanStatement) const
    {
        if (statementValue == nullptr)
            return;

        Indent();
        WriteKey(propertyKey);

        if (isBooleanStatement)
        {
            m_stream << "when";
        }

        for (auto i = 0; i < statementValue->numEntries; i++)
        {
            const auto& expEntry = statementValue->entries[i];

            if (i > 0)
                m_stream << " ";

            if (expEntry.type == EET_OPERATOR)
            {
                if (expEntry.data.op >= 0 && static_cast<unsigned>(expEntry.data.op) < std::extent_v<decltype(g_expFunctionNames)>)
                    m_stream << g_expFunctionNames[expEntry.data.op];
            }
            else
            {
                const auto& operand = expEntry.data.operand;

                switch (operand.dataType)
                {
                case VAL_FLOAT:
                    m_stream << operand.internals.floatVal;
                    break;

                case VAL_INT:
                    m_stream << operand.internals.intVal;
                    break;

                case VAL_STRING:
                    m_stream << "\"" << operand.internals.stringVal.string << "\"";
                    break;

                case VAL_FUNCTION:
                    m_stream << "FUNC";
                    break;

                default:
                    break;
                }
            }
        }

        m_stream << "\n";
    }

    void WriteMenuEventHandlerSetProperty(const std::string& propertyKey, const MenuEventHandlerSet* eventHandlerValue) const
    {
        if (eventHandlerValue == nullptr)
            return;

        Indent();
        WriteKey(propertyKey);
        m_stream << "\n";
    }

    void WriteItemKeyHandlerProperty(const ItemKeyHandler* itemKeyHandlerValue) const
    {
    }

    void WriteItemData(const itemDef_s* item)
    {
        WriteStringProperty("name", item->window.name);
        WriteStringProperty("text", item->text);
        WriteKeywordProperty("textsavegame", item->itemFlags & ITEM_FLAG_SAVE_GAME_INFO);
        WriteKeywordProperty("textcinematicsubtitle", item->itemFlags & ITEM_FLAG_CINEMATIC_SUBTITLE);
        WriteStringProperty("group", item->window.group);
        WriteRectProperty("rect", item->window.rect);
        WriteIntProperty("style", item->window.style, 0);
        WriteKeywordProperty("decoration", item->window.staticFlags & WINDOW_FLAG_DECORATION);
        WriteIntProperty("type", item->type, ITEM_TYPE_TEXT);
        WriteIntProperty("border", item->window.border, 0);
        WriteFloatProperty("borderSize", item->window.borderSize, 0.0f);
        WriteStatementProperty("visible", item->visibleExp, true);
        WriteStatementProperty("disabled", item->disabledExp, true);
        WriteIntProperty("ownerDraw", item->window.ownerDraw, 0);
        WriteIntProperty("align", item->alignment, 0);
        WriteIntProperty("textalign", item->textAlignMode, 0);
        WriteFloatProperty("textalignx", item->textalignx, 0.0f);
        WriteFloatProperty("textaligny", item->textaligny, 0.0f);
        WriteFloatProperty("textscale", item->textscale, 0.0f);
        WriteIntProperty("textstyle", item->textStyle, 0);
        WriteIntProperty("textfont", item->fontEnum, 0);
        WriteColorProperty("backcolor", item->window.backColor, COLOR_0000);
        WriteColorProperty("forecolor", item->window.foreColor, COLOR_1111);
        WriteColorProperty("bordercolor", item->window.borderColor, COLOR_0000);
        WriteColorProperty("outlinecolor", item->window.outlineColor, COLOR_0000);
        WriteColorProperty("disablecolor", item->window.disableColor, COLOR_0000);
        WriteColorProperty("disablecolor", item->glowColor, COLOR_0000);
        WriteMaterialProperty("background", item->window.background);
        WriteMenuEventHandlerSetProperty("onFocus", item->onFocus);
        WriteMenuEventHandlerSetProperty("leaveFocus", item->leaveFocus);
        WriteMenuEventHandlerSetProperty("mouseEnter", item->mouseEnter);
        WriteMenuEventHandlerSetProperty("mouseExit", item->mouseExit);
        WriteMenuEventHandlerSetProperty("mouseEnterText", item->mouseEnterText);
        WriteMenuEventHandlerSetProperty("mouseExitText", item->mouseExitText);
        WriteMenuEventHandlerSetProperty("action", item->action);
        WriteMenuEventHandlerSetProperty("accept", item->accept);
        WriteFloatProperty("special", item->special, 0.0f);
        WriteSoundAliasProperty("focusSound", item->focusSound);
        WriteFlagsProperty("ownerdrawFlag", item->window.ownerDrawFlags);
        WriteStringProperty("dvarTest", item->dvarTest);

        if (item->dvarFlags & ITEM_DVAR_FLAG_ENABLE)
            WriteStringProperty("enableDvar", item->enableDvar);
        else if (item->dvarFlags & ITEM_DVAR_FLAG_DISABLE)
            WriteStringProperty("disableDvar", item->enableDvar);
        else if (item->dvarFlags & ITEM_DVAR_FLAG_SHOW)
            WriteStringProperty("showDvar", item->enableDvar);
        else if (item->dvarFlags & ITEM_DVAR_FLAG_HIDE)
            WriteStringProperty("hideDvar", item->enableDvar);
        else if (item->dvarFlags & ITEM_DVAR_FLAG_FOCUS)
            WriteStringProperty("focusDvar", item->enableDvar);

        WriteItemKeyHandlerProperty(item->onKey);
        WriteStatementProperty("exp text", item->textExp, false);
        WriteStatementProperty("exp material", item->materialExp, false);
        WriteStatementProperty("exp disabled", item->disabledExp, false);
        // floatexpressions
        WriteIntProperty("gamemsgwindowindex", item->gameMsgWindowIndex, 0);
        WriteIntProperty("gamemsgwindowmode", item->gameMsgWindowMode, 0);
        // decodeEffect
    }

    void WriteItemDefs(const itemDef_s* const* itemDefs, const size_t itemCount)
    {
        for (auto i = 0u; i < itemCount; i++)
        {
            Indent();
            m_stream << "itemDef\n";
            Indent();
            m_stream << "{\n";
            IncIndent();

            WriteItemData(itemDefs[i]);

            DecIndent();
            Indent();
            m_stream << "}\n";
        }
    }

    void WriteMenuData(const menuDef_t* menu)
    {
        WriteStringProperty("name", menu->window.name);
        WriteBoolProperty("fullscreen", menu->fullScreen, false);
        WriteKeywordProperty("screenSpace", menu->window.staticFlags & WINDOW_FLAG_SCREEN_SPACE);
        WriteKeywordProperty("decoration", menu->window.staticFlags & WINDOW_FLAG_DECORATION);
        WriteRectProperty("rect", menu->window.rect);
        WriteIntProperty("style", menu->window.style, 0);
        WriteIntProperty("border", menu->window.border, 0);
        WriteFloatProperty("borderSize", menu->window.borderSize, 0.0f);
        WriteColorProperty("backcolor", menu->window.backColor, COLOR_0000);
        WriteColorProperty("forecolor", menu->window.foreColor, COLOR_1111);
        WriteColorProperty("bordercolor", menu->window.borderColor, COLOR_0000);
        WriteColorProperty("focuscolor", menu->focusColor, COLOR_0000);
        WriteMaterialProperty("background", menu->window.background);
        WriteIntProperty("ownerdraw", menu->window.ownerDraw, 0);
        WriteFlagsProperty("ownerdrawFlag", menu->window.ownerDrawFlags);
        WriteKeywordProperty("outOfBoundsClick", menu->window.staticFlags & WINDOW_FLAG_OUT_OF_BOUNDS_CLICK);
        WriteStringProperty("soundLoop", menu->soundName);
        WriteKeywordProperty("popup", menu->window.staticFlags & WINDOW_FLAG_POPUP);
        WriteFloatProperty("fadeClamp", menu->fadeClamp, 0.0f);
        WriteIntProperty("fadeCycle", menu->fadeCycle, 0);
        WriteFloatProperty("fadeAmount", menu->fadeAmount, 0.0f);
        WriteFloatProperty("fadeInAmount", menu->fadeInAmount, 0.0f);
        WriteFloatProperty("blurWorld", menu->blurRadius, 0.0f);
        WriteKeywordProperty("legacySplitScreenScale", menu->window.staticFlags & WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE);
        WriteKeywordProperty("hiddenDuringScope", menu->window.staticFlags & WINDOW_FLAG_HIDDEN_DURING_SCOPE);
        WriteKeywordProperty("hiddenDuringFlashbang", menu->window.staticFlags & WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG);
        WriteKeywordProperty("hiddenDuringUI", menu->window.staticFlags & WINDOW_FLAG_HIDDEN_DURING_UI);
        WriteStringProperty("allowedBinding", menu->allowedBinding);
        WriteKeywordProperty("textOnlyFocus", menu->window.staticFlags & WINDOW_FLAG_TEXT_ONLY_FOCUS);
        WriteStatementProperty("visible", menu->visibleExp, true);
        WriteStatementProperty("exp rect X", menu->rectXExp, false);
        WriteStatementProperty("exp rect Y", menu->rectYExp, false);
        WriteStatementProperty("exp rect W", menu->rectWExp, false);
        WriteStatementProperty("exp rect H", menu->rectHExp, false);
        WriteStatementProperty("exp openSound", menu->openSoundExp, false);
        WriteStatementProperty("exp closeSound", menu->closeSoundExp, false);
        WriteMenuEventHandlerSetProperty("onOpen", menu->onOpen);
        WriteMenuEventHandlerSetProperty("onClose", menu->onClose);
        WriteMenuEventHandlerSetProperty("onRequestClose", menu->onCloseRequest);
        WriteMenuEventHandlerSetProperty("onESC", menu->onESC);
        WriteItemKeyHandlerProperty(menu->onKey);
        WriteItemDefs(menu->items, menu->itemCount);
    }

public:
    explicit MenuDumperIw4(std::ostream& stream)
        : MenuDumper(stream)
    {
    }

    void WriteMenu(const menuDef_t* menu)
    {
        Indent();
        m_stream << "menuDef\n";
        Indent();
        m_stream << "{\n";
        IncIndent();

        WriteMenuData(menu);

        DecIndent();
        Indent();
        m_stream << "}\n";
    }
};

const MenuList* AssetDumperMenuDef::GetParentMenuList(XAssetInfo<menuDef_t>* asset)
{
    const auto* menu = asset->Asset();
    const auto* gameAssetPool = dynamic_cast<GameAssetPoolIW4*>(asset->m_zone->m_pools.get());
    for (const auto* menuList : *gameAssetPool->m_menu_list)
    {
        const auto* menuListAsset = menuList->Asset();

        for (auto menuIndex = 0; menuIndex < menuListAsset->menuCount; menuIndex++)
        {
            if (menuListAsset->menus[menuIndex] == menu)
                return menuListAsset;
        }
    }

    return nullptr;
}

std::string AssetDumperMenuDef::GetPathForMenu(XAssetInfo<menuDef_t>* asset)
{
    const auto* list = GetParentMenuList(asset);

    if (!list)
        return "ui_mp/" + std::string(asset->Asset()->window.name) + ".menu";

    const fs::path p(list->name);
    std::string parentPath;
    if (p.has_parent_path())
        parentPath = p.parent_path().string() + "/";

    return parentPath + std::string(asset->Asset()->window.name) + ".menu";
}

bool AssetDumperMenuDef::ShouldDump(XAssetInfo<menuDef_t>* asset)
{
    return true;
}

void AssetDumperMenuDef::DumpAsset(AssetDumpingContext& context, XAssetInfo<menuDef_t>* asset)
{
    const auto* menu = asset->Asset();
    const auto assetFile = context.OpenAssetFile(GetPathForMenu(asset));

    if (!assetFile)
        return;

    MenuDumperIw4 menuDumper(*assetFile);

    menuDumper.Start();
    menuDumper.WriteMenu(menu);
    menuDumper.End();
}
