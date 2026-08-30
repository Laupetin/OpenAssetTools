#include "MenuWriterT4.h"

#include "Game/T4/MenuConstantsT4.h"
#include "Menu/AbstractMenuWriter.h"
#include "ObjWriting.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <sstream>

using namespace T4;

namespace
{
    // Set this to true to skip interpretative expression dumping
    constexpr auto DUMP_NAIVE = false;

    size_t FindStatementClosingParenthesis(const statement_s& statement, const size_t openingParenthesisPosition)
    {
        assert(statement.numEntries >= 0);
        assert(openingParenthesisPosition < static_cast<size_t>(statement.numEntries));

        const auto statementEnd = static_cast<size_t>(statement.numEntries);

        // The openingParenthesisPosition does not necessarily point to an actual opening parenthesis operator. That's fine though.
        // We will pretend it does since the game does sometimes leave out opening parenthesis from the entries.
        auto currentParenthesisDepth = 1;
        for (auto currentSearchPosition = openingParenthesisPosition + 1; currentSearchPosition < statementEnd; currentSearchPosition++)
        {
            const auto* expEntry = statement.entries[currentSearchPosition];
            if (!expEntry || expEntry->type != EET_OPERATOR)
                continue;

            // Any function means a "left out" left paren
            if (expEntry->data.op == OP_LEFTPAREN || expEntry->data.op >= OP_FIRSTFUNCTIONCALL)
            {
                currentParenthesisDepth++;
            }
            else if (expEntry->data.op == OP_RIGHTPAREN)
            {
                if (currentParenthesisDepth > 0)
                    currentParenthesisDepth--;
                if (currentParenthesisDepth == 0)
                    return currentSearchPosition;
            }
        }

        return statementEnd;
    }

    class MenuWriter final : public menu::AbstractBaseWriter, public menu::IWriterT4
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

        void WriteStatementNaive(const statement_s& statement) const
        {
            const auto entryCount = static_cast<size_t>(statement.numEntries);

            const auto missingClosingParenthesis = statement.numEntries > 0 && statement.entries[0]->type == EET_OPERATOR
                                                   && statement.entries[0]->data.op == OP_LEFTPAREN
                                                   && FindStatementClosingParenthesis(statement, 0) >= static_cast<size_t>(statement.numEntries);

            for (auto i = 0uz; i < entryCount; i++)
            {
                const auto& entry = statement.entries[i];
                if (entry->type == EET_OPERAND)
                {
                    size_t pos = i;
                    bool discard = false;
                    WriteStatementOperand(statement, pos, discard);
                }
                else
                {
                    assert(entry->data.op >= 0 && static_cast<unsigned>(entry->data.op) < std::extent_v<decltype(g_expFunctionNames)>);
                    if (entry->data.op >= 0 && static_cast<unsigned>(entry->data.op) < std::extent_v<decltype(g_expFunctionNames)>)
                        m_stream << g_expFunctionNames[entry->data.op];
                    if (entry->data.op >= OP_FIRSTFUNCTIONCALL)
                        m_stream << "(";
                }
            }

            if (missingClosingParenthesis)
                m_stream << ")";
        }

        void WriteStatementOperator(const statement_s& statement, size_t& currentPos, bool& spaceNext) const
        {
            const auto& expEntry = statement.entries[currentPos];

            if (spaceNext && expEntry->data.op != OP_COMMA)
                m_stream << " ";

            if (expEntry->data.op == OP_LEFTPAREN)
            {
                const auto closingParenPos = FindStatementClosingParenthesis(statement, currentPos);
                m_stream << "(";
                WriteStatementEntryRange(statement, currentPos + 1, closingParenPos);
                m_stream << ")";

                currentPos = closingParenPos + 1;
                spaceNext = true;
            }
            else
            {
                if (expEntry->data.op >= 0 && static_cast<unsigned>(expEntry->data.op) < std::extent_v<decltype(g_expFunctionNames)>)
                    m_stream << g_expFunctionNames[expEntry->data.op];

                if (expEntry->data.op >= OP_FIRSTFUNCTIONCALL)
                {
                    // Functions do not have opening parenthesis in the entries. We can just pretend they do though
                    const auto closingParenPos = FindStatementClosingParenthesis(statement, currentPos);
                    m_stream << "(";
                    WriteStatementEntryRange(statement, currentPos + 1, closingParenPos);
                    m_stream << ")";
                    currentPos = closingParenPos + 1;
                }
                else
                    currentPos++;

                spaceNext = expEntry->data.op != OP_NOT;
            }
        }

        void WriteStatementOperand(const statement_s& statement, size_t& currentPos, bool& spaceNext) const
        {
            const auto& expEntry = statement.entries[currentPos];

            if (spaceNext)
                m_stream << " ";

            const auto& operand = expEntry->data.operand;

            switch (operand.dataType)
            {
            case VAL_FLOAT:
                m_stream << operand.internals.floatVal;
                break;

            case VAL_INT:
                m_stream << operand.internals.intVal;
                break;

            case VAL_STRING:
                WriteEscapedString(operand.internals.string);
                break;

            default:
                break;
            }

            currentPos++;
            spaceNext = true;
        }

        void WriteStatementEntryRange(const statement_s& statement, const size_t startOffset, const size_t endOffset) const
        {
            assert(startOffset <= endOffset);
            assert(endOffset <= static_cast<size_t>(statement.numEntries));

            auto currentPos = startOffset;
            auto spaceNext = false;
            while (currentPos < endOffset)
            {
                const auto& expEntry = statement.entries[currentPos];

                if (expEntry->type == EET_OPERATOR)
                {
                    WriteStatementOperator(statement, currentPos, spaceNext);
                }
                else
                {
                    WriteStatementOperand(statement, currentPos, spaceNext);
                }
            }
        }

        void WriteStatement(const statement_s& statement) const
        {
            if (!HasStatement(statement))
                return;

            WriteStatementEntryRange(statement, 0, static_cast<size_t>(statement.numEntries));
        }

        void WriteStatementSkipInitialUnnecessaryParenthesis(const statement_s& statement) const
        {
            if (!HasStatement(statement))
                return;

            const auto statementEnd = static_cast<size_t>(statement.numEntries);

            if (statement.numEntries >= 1 && statement.entries[0]->type == EET_OPERATOR && statement.entries[0]->data.op == OP_LEFTPAREN)
            {
                const auto parenthesisEnd = FindStatementClosingParenthesis(statement, 0);

                if (parenthesisEnd >= statementEnd)
                    WriteStatementEntryRange(statement, 1, statementEnd);
                else if (parenthesisEnd == statementEnd - 1)
                    WriteStatementEntryRange(statement, 1, statementEnd - 1);
                else
                    WriteStatementEntryRange(statement, 0, statementEnd);
            }
            else
            {
                WriteStatementEntryRange(statement, 0, statementEnd);
            }
        }

        void WriteStatementProperty(const std::string& propertyKey, const statement_s& statement, const bool isBooleanStatement) const
        {
            if (!HasStatement(statement))
                return;

            Indent();
            WriteKey(propertyKey);

            if (isBooleanStatement)
            {
                m_stream << "when(";
                if constexpr (DUMP_NAIVE)
                    WriteStatementNaive(statement);
                else
                    WriteStatementSkipInitialUnnecessaryParenthesis(statement);
                m_stream << ");\n";
            }
            else
            {
                if constexpr (DUMP_NAIVE)
                    WriteStatementNaive(statement);
                else
                    WriteStatement(statement);
                m_stream << ";\n";
            }
        }

        // #define WRITE_ORIGINAL_SCRIPT
        void WriteUnconditionalScript(const char* script) const
        {
#ifdef WRITE_ORIGINAL_SCRIPT
            Indent();
            m_stream << script << "\n";
            return;
#endif

            const auto tokenList = CreateScriptTokenList(script);

            auto isNewStatement = true;
            for (const auto& token : tokenList)
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
            for (const auto* current = handler; current; current = current->next)
            {
                std::string key;
                if (current->key >= '!' && current->key <= '~' && current->key != '"')
                    key = std::format("execKey \"{}\"", static_cast<char>(current->key));
                else
                    key = std::format("execKeyInt {}", current->key);

                WriteScriptProperty(key, current->action);
            }
        }

        void WriteMultiTokenStringProperty(const std::string& propertyKey, const char* value) const
        {
            if (!value)
                return;

            Indent();
            WriteKey(propertyKey);
            m_stream << "{ ";

            const auto tokenList = CreateScriptTokenList(value);
            auto firstToken = true;
            for (const auto& token : tokenList)
            {
                if (firstToken)
                    firstToken = false;
                else
                    m_stream << ";";

                WriteEscapedString(token);
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
            m_stream << listBox.numColumns << "\n";

            const auto columnCount = std::min<size_t>(listBox.numColumns, std::size(listBox.columnInfo));
            for (size_t columnIndex = 0u; columnIndex < columnCount; columnIndex++)
            {
                const auto& column = listBox.columnInfo[columnIndex];

                Indent();
                for (auto i = 0u; i < MENU_KEY_SPACING; i++)
                    m_stream << " ";

                m_stream << column.pos << " " << column.width << " " << column.maxChars << " " << column.alignment << "\n";
            }
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
            WriteScriptProperty("doubleclick", listBox.onDoubleClick);
            WriteScriptProperty("onListboxSelectionChange", item.onListboxSelectionChange);
            WriteColorProperty("selectBorder", listBox.selectBorder, COLOR_0000);
            WriteColorProperty("disableColor", listBox.disableColor, COLOR_0000);
            WriteColorProperty("focusColor", listBox.focusColor, COLOR_0000);
            WriteMaterialProperty("selectIcon", listBox.selectIcon);
            WriteMaterialProperty("backgroundItemListbox", listBox.backgroundItemListbox);
            WriteMaterialProperty("highlightTexture", listBox.highlightTexture);
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
            const auto valueCount = std::min<size_t>(multi.count, std::size(multi.dvarValue));
            for (size_t valueIndex = 0u; valueIndex < valueCount; valueIndex++)
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

        void WriteItemTextProperty(const char* text) const
        {
            // T4 distinguishes explicitly empty text from null text, which falls back to the item's dvar.
            if (!text)
                return;

            Indent();
            WriteKey("text");
            WriteEscapedString(text);
            m_stream << "\n";
        }

        void WriteItemData(const itemDef_s& item)
        {
            WriteStringProperty("name", item.window.name);
            WriteItemTextProperty(item.text);
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

        void WriteItemDefs(const itemDef_s* const* items, const size_t itemCount)
        {
            if (!items || itemCount <= 0)
                return;

            for (size_t itemIndex = 0u; itemIndex < itemCount; itemIndex++)
            {
                const auto* item = items[itemIndex];
                if (!item)
                    continue;

                StartItemDefScope();
                WriteItemData(*item);
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
            WriteScriptProperty("onFocus", menu.onFocus);
            WriteScriptProperty("onClose", menu.onClose);
            WriteScriptProperty("onESC", menu.onESC);
            WriteItemKeyHandlers(menu.onKey);
            WriteItemDefs(menu.items, menu.itemCount);
        }
    };
} // namespace

namespace menu
{
    std::unique_ptr<IWriterT4> CreateMenuWriterT4(std::ostream& stream)
    {
        return std::make_unique<MenuWriter>(stream);
    }
} // namespace menu
