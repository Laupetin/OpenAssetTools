#include "MenuWriterIW3.h"

#include "Menu/AbstractMenuWriter.h"

#include <array>
#include <cmath>
#include <limits>
#include <sstream>

using namespace IW3;

namespace
{
    constexpr std::array<const char*, NUM_OPERATORS> EXPRESSION_OPERATION_NAMES{
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
        "scoped",
        "scoreboard_visible",
        "inkillcam",
        "player",
        "selecting_location",
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
        "secondsastime",
        "tablelookup",
        "locstring",
        "localvarint",
        "localvarbool",
        "localvarfloat",
        "localvarstring",
        "timeleft",
        "secondsascountdown",
        "gamemsgwndactive",
        "int",
        "string",
        "float",
        "gametypename",
        "gametype",
        "gametypedescription",
        "scoreatrank",
        "friendsonline",
        "spectatingclient",
        "statrangeanybitsset",
        "keybinding",
        "actionslotusable",
        "hudfade",
        "maxrecommendedplayers",
        "acceptinginvite",
        "isintermission",
    };

    class MenuWriter final : public menu::AbstractBaseWriter, public menu::IWriterIW3
    {
    public:
        explicit MenuWriter(std::ostream& stream)
            : AbstractBaseWriter(stream)
        {
        }

        void WriteMenu(const menuDef_t& menu) override
        {
            StartMenuDefScope();
            WriteMenuData(menu);
            EndScope();
        }

        void Start() override
        {
            AbstractBaseWriter::Start();
        }

        void End() override
        {
            AbstractBaseWriter::End();
        }

        void IncludeMenu(const std::string& menuPath) const override
        {
            AbstractBaseWriter::IncludeMenu(menuPath);
        }

    private:
        static bool HasStatement(const statement_s& statement)
        {
            return statement.numEntries > 0 && statement.entries;
        }

        void WriteStatement(const statement_s& statement) const
        {
            auto parenthesisDepth = 0;

            for (auto entryIndex = 0; entryIndex < statement.numEntries; entryIndex++)
            {
                const auto* entry = statement.entries[entryIndex];
                if (!entry)
                    continue;

                if (entry->type == EET_OPERAND)
                {
                    switch (entry->data.operand.dataType)
                    {
                    case VAL_INT:
                        m_stream << entry->data.operand.internals.intVal;
                        break;

                    case VAL_FLOAT:
                        m_stream << entry->data.operand.internals.floatVal;
                        break;

                    case VAL_STRING:
                        WriteEscapedString(entry->data.operand.internals.stringVal ? entry->data.operand.internals.stringVal : "");
                        break;

                    default:
                        m_stream << "0";
                        break;
                    }

                    continue;
                }

                const auto operation = entry->data.op;
                if (operation < 0 || static_cast<size_t>(operation) >= EXPRESSION_OPERATION_NAMES.size())
                {
                    m_stream << "0";
                    continue;
                }

                if (operation == OP_NOOP)
                {
                    m_stream << "0";
                }
                else if (operation >= OP_SIN)
                {
                    m_stream << EXPRESSION_OPERATION_NAMES[operation] << "(";
                    parenthesisDepth++;
                }
                else
                {
                    m_stream << EXPRESSION_OPERATION_NAMES[operation];

                    if (operation == OP_LEFTPAREN)
                        parenthesisDepth++;
                    else if (operation == OP_RIGHTPAREN && parenthesisDepth > 0)
                        parenthesisDepth--;
                }
            }

            while (parenthesisDepth > 0)
            {
                m_stream << ")";
                parenthesisDepth--;
            }
        }

        void WriteStatementProperty(const std::string& propertyKey, const statement_s& statement, const bool isBooleanStatement) const
        {
            if (!HasStatement(statement))
                return;

            Indent();
            WriteKey(propertyKey);
            if (isBooleanStatement)
                m_stream << "when(";

            WriteStatement(statement);

            if (isBooleanStatement)
                m_stream << ")";
            m_stream << ";\n";
        }

        void WriteUnconditionalScript(const char* script) const
        {
            const auto tokens = CreateScriptTokenList(script);
            auto isNewStatement = true;

            for (const auto& token : tokens)
            {
                if (isNewStatement)
                {
                    if (token == ";")
                        continue;
                    Indent();
                }

                if (token == ";")
                {
                    m_stream << ";\n";
                    isNewStatement = true;
                    continue;
                }

                if (!isNewStatement)
                    m_stream << " ";
                else
                    isNewStatement = false;

                if (DoesTokenNeedQuotationMarks(token))
                    WriteEscapedString(token);
                else
                    m_stream << token;
            }

            if (!isNewStatement)
                m_stream << ";\n";
        }

        void WriteScriptProperty(const std::string& propertyKey, const char* script)
        {
            if (!script || !script[0])
                return;

            Indent();
            m_stream << propertyKey << "\n";
            Indent();
            m_stream << "{\n";
            IncIndent();
            WriteUnconditionalScript(script);
            DecIndent();
            Indent();
            m_stream << "}\n";
        }

        void WriteRectProperty(const std::string& propertyKey, const rectDef_s& rect) const
        {
            Indent();
            WriteKey(propertyKey);
            m_stream << rect.x << " " << rect.y << " " << rect.w << " " << rect.h << " " << rect.horzAlign << " " << rect.vertAlign << "\n";
        }

        void WriteMaterialProperty(const std::string& propertyKey, const Material* material) const
        {
            if (!material || !material->info.name)
                return;

            const auto* materialName = material->info.name;
            if (materialName[0] == ',')
                materialName++;
            WriteStringProperty(propertyKey, materialName);
        }

        void WriteSoundAliasProperty(const std::string& propertyKey, const snd_alias_list_t* soundAlias) const
        {
            if (soundAlias)
                WriteStringProperty(propertyKey, soundAlias->aliasName);
        }

        void WriteItemKeyHandlers(const ItemKeyHandler* handler)
        {
            for (auto* current = handler; current; current = current->next)
            {
                std::ostringstream key;
                if (current->key >= '!' && current->key <= '~' && current->key != '"')
                    key << "execKey \"" << static_cast<char>(current->key) << "\"";
                else
                    key << "execKeyInt " << current->key;

                WriteScriptProperty(key.str(), current->action);
            }
        }

        void WriteMultiTokenStringProperty(const std::string& propertyKey, const char* value) const
        {
            if (!value)
                return;

            Indent();
            WriteKey(propertyKey);
            m_stream << "{ ";

            auto firstToken = true;
            for (const auto& token : CreateScriptTokenList(value))
            {
                if (!firstToken)
                    m_stream << ";";
                WriteEscapedString(token);
                firstToken = false;
            }

            if (!firstToken)
                m_stream << " ";
            m_stream << "}\n";
        }

        void WriteColumnProperty(const listBoxDef_s& listBox) const
        {
            if (listBox.numColumns <= 0)
                return;

            Indent();
            WriteKey("columns");
            m_stream << listBox.numColumns;
            for (auto columnIndex = 0; columnIndex < listBox.numColumns && columnIndex < 16; columnIndex++)
            {
                const auto& column = listBox.columnInfo[columnIndex];
                m_stream << " " << column.pos << " " << column.width << " " << column.maxChars << " " << column.alignment;
            }
            m_stream << "\n";
        }

        void WriteListBoxProperties(const itemDef_s& item)
        {
            if (item.type != ITEM_TYPE_LISTBOX || !item.typeData.listBox)
                return;

            const auto& listBox = *item.typeData.listBox;
            WriteKeywordProperty("notselectable", listBox.notselectable != 0);
            WriteKeywordProperty("noscrollbars", listBox.noScrollBars != 0);
            WriteKeywordProperty("usepaging", listBox.usePaging != 0);
            WriteFloatProperty("elementwidth", listBox.elementWidth, 0.0f);
            WriteFloatProperty("elementheight", listBox.elementHeight, 0.0f);
            WriteFloatProperty("feeder", item.special, 0.0f);
            WriteIntProperty("elementtype", listBox.elementStyle, 0);
            WriteColumnProperty(listBox);
            WriteScriptProperty("doubleclick", listBox.doubleClick);
            WriteColorProperty("selectBorder", listBox.selectBorder, COLOR_0000);
            WriteColorProperty("disableColor", listBox.disableColor, COLOR_0000);
            WriteMaterialProperty("selectIcon", listBox.selectIcon);
        }

        void WriteDvarFloatProperty(const itemDef_s& item, const editFieldDef_s& editField) const
        {
            if (!item.dvar)
                return;

            Indent();
            WriteKey("dvarFloat");
            WriteEscapedString(item.dvar);
            m_stream << " " << editField.defVal << " " << editField.minVal << " " << editField.maxVal << "\n";
        }

        void WriteEditFieldProperties(const itemDef_s& item) const
        {
            switch (item.type)
            {
            case ITEM_TYPE_TEXT:
            case ITEM_TYPE_EDITFIELD:
            case ITEM_TYPE_NUMERICFIELD:
            case ITEM_TYPE_SLIDER:
            case ITEM_TYPE_YESNO:
            case ITEM_TYPE_BIND:
            case ITEM_TYPE_VALIDFILEFIELD:
            case ITEM_TYPE_DECIMALFIELD:
            case ITEM_TYPE_UPREDITFIELD:
                break;

            default:
                return;
            }

            if (!item.typeData.editField)
                return;

            const auto& editField = *item.typeData.editField;
            if (std::fabs(-1.0f - editField.defVal) >= std::numeric_limits<float>::epsilon()
                || std::fabs(-1.0f - editField.minVal) >= std::numeric_limits<float>::epsilon()
                || std::fabs(-1.0f - editField.maxVal) >= std::numeric_limits<float>::epsilon())
            {
                WriteDvarFloatProperty(item, editField);
            }
            else
            {
                WriteStringProperty("dvar", item.dvar);
            }

            WriteIntProperty("maxChars", editField.maxChars, 0);
            WriteKeywordProperty("maxCharsGotoNext", editField.maxCharsGotoNext != 0);
            WriteIntProperty("maxPaintChars", editField.maxPaintChars, 0);
        }

        void WriteMultiValueProperty(const multiDef_s& multi) const
        {
            if (multi.count <= 0)
                return;

            Indent();
            WriteKey(multi.strDef ? "dvarStrList" : "dvarFloatList");
            m_stream << "{";
            for (auto valueIndex = 0; valueIndex < multi.count && valueIndex < 32; valueIndex++)
            {
                if (!multi.dvarList[valueIndex] || (multi.strDef && !multi.dvarStr[valueIndex]))
                    continue;

                m_stream << " ";
                WriteEscapedString(multi.dvarList[valueIndex]);
                m_stream << " ";
                if (multi.strDef)
                    WriteEscapedString(multi.dvarStr[valueIndex]);
                else
                    m_stream << multi.dvarValue[valueIndex];
            }
            m_stream << " }\n";
        }

        void WriteMultiProperties(const itemDef_s& item) const
        {
            if (item.type != ITEM_TYPE_MULTI || !item.typeData.multi)
                return;

            WriteStringProperty("dvar", item.dvar);
            WriteMultiValueProperty(*item.typeData.multi);
        }

        void WriteEnumDvarProperties(const itemDef_s& item) const
        {
            if (item.type != ITEM_TYPE_DVARENUM)
                return;

            WriteStringProperty("dvar", item.dvar);
            WriteStringProperty("dvarEnumList", item.typeData.enumDvarName);
        }

        void WriteItemData(const itemDef_s& item)
        {
            WriteStringProperty("name", item.window.name);
            WriteStringProperty("text", item.text);
            WriteStringProperty("group", item.window.group);
            WriteRectProperty("rect", item.window.rectClient);
            WriteIntProperty("style", item.window.style, 0);
            WriteKeywordProperty("decoration", item.window.staticFlags & WINDOW_FLAG_DECORATION);
            WriteKeywordProperty("autowrapped", item.window.staticFlags & WINDOW_FLAG_AUTO_WRAPPED);
            WriteKeywordProperty("horizontalscroll", item.window.staticFlags & WINDOW_FLAG_HORIZONTAL_SCROLL);
            WriteIntProperty("type", item.type, ITEM_TYPE_TEXT);
            WriteIntProperty("border", item.window.border, 0);
            WriteFloatProperty("borderSize", item.window.borderSize, 0.0f);

            if (HasStatement(item.visibleExp))
                WriteStatementProperty("visible", item.visibleExp, true);
            else if (item.window.dynamicFlags[0] & WINDOW_FLAG_VISIBLE)
                WriteIntProperty("visible", 1, 0);

            WriteIntProperty("ownerdraw", item.window.ownerDraw, 0);
            WriteFlagsProperty("ownerdrawFlag", item.window.ownerDrawFlags);
            WriteIntProperty("align", item.alignment, 0);
            WriteIntProperty("textalign", item.textAlignMode, 0);
            WriteFloatProperty("textalignx", item.textalignx, 0.0f);
            WriteFloatProperty("textaligny", item.textaligny, 0.0f);
            WriteFloatProperty("textscale", item.textscale, 0.0f);
            WriteIntProperty("textstyle", item.textStyle, 0);
            WriteIntProperty("textfont", item.fontEnum, 0);
            WriteColorProperty("backcolor", item.window.backColor, COLOR_0000);
            WriteColorProperty("forecolor", item.window.foreColor, COLOR_1111);
            WriteColorProperty("bordercolor", item.window.borderColor, COLOR_0000);
            WriteColorProperty("outlinecolor", item.window.outlineColor, COLOR_0000);
            WriteMaterialProperty("background", item.window.background);
            WriteScriptProperty("onFocus", item.onFocus);
            WriteScriptProperty("leaveFocus", item.leaveFocus);
            WriteScriptProperty("mouseEnter", item.mouseEnter);
            WriteScriptProperty("mouseExit", item.mouseExit);
            WriteScriptProperty("mouseEnterText", item.mouseEnterText);
            WriteScriptProperty("mouseExitText", item.mouseExitText);
            WriteScriptProperty("action", item.action);
            WriteScriptProperty("accept", item.onAccept);
            WriteSoundAliasProperty("focusSound", item.focusSound);
            WriteStringProperty("dvarTest", item.dvarTest);

            if (item.dvarFlags & ITEM_DVAR_FLAG_ENABLE)
                WriteMultiTokenStringProperty("enableDvar", item.enableDvar);
            else if (item.dvarFlags & ITEM_DVAR_FLAG_DISABLE)
                WriteMultiTokenStringProperty("disableDvar", item.enableDvar);
            else if (item.dvarFlags & ITEM_DVAR_FLAG_SHOW)
                WriteMultiTokenStringProperty("showDvar", item.enableDvar);
            else if (item.dvarFlags & ITEM_DVAR_FLAG_HIDE)
                WriteMultiTokenStringProperty("hideDvar", item.enableDvar);
            else if (item.dvarFlags & ITEM_DVAR_FLAG_FOCUS)
                WriteMultiTokenStringProperty("focusDvar", item.enableDvar);

            WriteItemKeyHandlers(item.onKey);
            WriteStatementProperty("exp text", item.textExp, false);
            WriteStatementProperty("exp material", item.materialExp, false);
            WriteStatementProperty("exp rect X", item.rectXExp, false);
            WriteStatementProperty("exp rect Y", item.rectYExp, false);
            WriteStatementProperty("exp rect W", item.rectWExp, false);
            WriteStatementProperty("exp rect H", item.rectHExp, false);
            WriteStatementProperty("exp forecolor A", item.forecolorAExp, false);
            WriteIntProperty("gamemsgwindowindex", item.gameMsgWindowIndex, 0);
            WriteIntProperty("gamemsgwindowmode", item.gameMsgWindowMode, 0);

            WriteListBoxProperties(item);
            WriteEditFieldProperties(item);
            WriteMultiProperties(item);
            WriteEnumDvarProperties(item);
        }

        void WriteItemDefs(itemDef_s* const* items, const int itemCount)
        {
            if (!items || itemCount <= 0)
                return;

            for (auto itemIndex = 0; itemIndex < itemCount; itemIndex++)
            {
                if (!items[itemIndex])
                    continue;

                StartItemDefScope();
                WriteItemData(*items[itemIndex]);
                EndScope();
            }
        }

        void WriteMenuData(const menuDef_t& menu)
        {
            WriteStringProperty("name", menu.window.name);
            WriteBoolProperty("fullscreen", menu.fullScreen != 0, false);
            WriteKeywordProperty("decoration", menu.window.staticFlags & WINDOW_FLAG_DECORATION);
            WriteRectProperty("rect", menu.window.rect);
            WriteIntProperty("style", menu.window.style, 0);
            WriteIntProperty("border", menu.window.border, 0);
            WriteFloatProperty("borderSize", menu.window.borderSize, 0.0f);
            WriteColorProperty("backcolor", menu.window.backColor, COLOR_0000);
            WriteColorProperty("forecolor", menu.window.foreColor, COLOR_1111);
            WriteColorProperty("bordercolor", menu.window.borderColor, COLOR_0000);
            WriteColorProperty("focuscolor", menu.focusColor, COLOR_0000);
            WriteColorProperty("disablecolor", menu.disableColor, COLOR_0000);
            WriteColorProperty("outlinecolor", menu.window.outlineColor, COLOR_0000);
            WriteMaterialProperty("background", menu.window.background);
            WriteIntProperty("ownerdraw", menu.window.ownerDraw, 0);
            WriteFlagsProperty("ownerdrawFlag", menu.window.ownerDrawFlags);
            WriteKeywordProperty("outOfBoundsClick", menu.window.staticFlags & WINDOW_FLAG_OUT_OF_BOUNDS_CLICK);
            WriteStringProperty("soundLoop", menu.soundName);
            WriteKeywordProperty("popup", menu.window.staticFlags & WINDOW_FLAG_POPUP);
            WriteFloatProperty("fadeClamp", menu.fadeClamp, 0.0f);
            WriteIntProperty("fadeCycle", menu.fadeCycle, 0);
            WriteFloatProperty("fadeAmount", menu.fadeAmount, 0.0f);
            WriteFloatProperty("fadeInAmount", menu.fadeInAmount, 0.0f);
            WriteFloatProperty("blurWorld", menu.blurRadius, 0.0f);
            WriteKeywordProperty("legacySplitScreenScale", menu.window.staticFlags & WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE);
            WriteKeywordProperty("hiddenDuringScope", menu.window.staticFlags & WINDOW_FLAG_HIDDEN_DURING_SCOPE);
            WriteKeywordProperty("hiddenDuringFlashbang", menu.window.staticFlags & WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG);
            WriteKeywordProperty("hiddenDuringUI", menu.window.staticFlags & WINDOW_FLAG_HIDDEN_DURING_UI);
            WriteStringProperty("allowedBinding", menu.allowedBinding);

            if (HasStatement(menu.visibleExp))
                WriteStatementProperty("visible", menu.visibleExp, true);
            else if (menu.window.dynamicFlags[0] & WINDOW_FLAG_VISIBLE)
                WriteIntProperty("visible", 1, 0);

            WriteStatementProperty("exp rect X", menu.rectXExp, false);
            WriteStatementProperty("exp rect Y", menu.rectYExp, false);
            WriteScriptProperty("onOpen", menu.onOpen);
            WriteScriptProperty("onClose", menu.onClose);
            WriteScriptProperty("onESC", menu.onESC);
            WriteItemKeyHandlers(menu.onKey);
            WriteItemDefs(menu.items, menu.itemCount);
        }
    };
} // namespace

namespace menu
{
    std::unique_ptr<IWriterIW3> CreateMenuWriterIW3(std::ostream& stream)
    {
        return std::make_unique<MenuWriter>(stream);
    }
} // namespace menu
