#include "MenuDumperIW5.h"

#include <cmath>
#include <cassert>
#include <sstream>

#include "ObjWriting.h"
#include "Game/IW5/MenuConstantsIW5.h"

using namespace IW5;

size_t MenuDumper::FindStatementClosingParenthesis(const Statement_s* statement, size_t openingParenthesisPosition)
{
    assert(statement->numEntries >= 0);
    assert(openingParenthesisPosition < static_cast<size_t>(statement->numEntries));

    const auto statementEnd = static_cast<size_t>(statement->numEntries);

    // The openingParenthesisPosition does not necessarily point to an actual opening parenthesis operator. That's fine though.
    // We will pretend it does since the game does sometimes leave out opening parenthesis from the entries.
    auto currentParenthesisDepth = 1;
    for (auto currentSearchPosition = openingParenthesisPosition + 1; currentSearchPosition < statementEnd; currentSearchPosition++)
    {
        const auto& expEntry = statement->entries[currentSearchPosition];
        if (expEntry.type != EET_OPERATOR)
            continue;

        // Any function means a "left out" left paren
        if (expEntry.data.op == OP_LEFTPAREN || expEntry.data.op >= OP_COUNT)
        {
            currentParenthesisDepth++;
        }
        else if (expEntry.data.op == OP_RIGHTPAREN)
        {
            if (currentParenthesisDepth > 0)
                currentParenthesisDepth--;
            if (currentParenthesisDepth == 0)
                return currentSearchPosition;
        }
    }

    return statementEnd;
}

void MenuDumper::WriteStatementOperator(const Statement_s* statement, size_t& currentPos, bool& spaceNext) const
{
    const auto& expEntry = statement->entries[currentPos];

    if (spaceNext && expEntry.data.op != OP_COMMA)
        m_stream << " ";

    if (expEntry.data.op == OP_LEFTPAREN)
    {
        const auto closingParenPos = FindStatementClosingParenthesis(statement, currentPos);
        m_stream << "(";
        WriteStatementEntryRange(statement, currentPos + 1, closingParenPos);
        m_stream << ")";

        currentPos = closingParenPos + 1;
        spaceNext = true;
    }
    else if (expEntry.data.op >= EXP_FUNC_STATIC_DVAR_INT && expEntry.data.op <= EXP_FUNC_STATIC_DVAR_STRING)
    {
        switch (expEntry.data.op)
        {
        case EXP_FUNC_STATIC_DVAR_INT:
            m_stream << "dvarint";
            break;

        case EXP_FUNC_STATIC_DVAR_BOOL:
            m_stream << "dvarbool";
            break;

        case EXP_FUNC_STATIC_DVAR_FLOAT:
            m_stream << "dvarfloat";
            break;

        case EXP_FUNC_STATIC_DVAR_STRING:
            m_stream << "dvarstring";
            break;

        default:
            break;
        }

        // Functions do not have opening parenthesis in the entries. We can just pretend they do though
        const auto closingParenPos = FindStatementClosingParenthesis(statement, currentPos);
        m_stream << "(";

        if (closingParenPos - currentPos + 1 >= 1)
        {
            const auto& staticDvarEntry = statement->entries[currentPos + 1];
            if (staticDvarEntry.type == EET_OPERAND && staticDvarEntry.data.operand.dataType == VAL_INT)
            {
                if (statement->supportingData
                    && statement->supportingData->staticDvarList.staticDvars
                    && staticDvarEntry.data.operand.internals.intVal >= 0
                    && staticDvarEntry.data.operand.internals.intVal < statement->supportingData->staticDvarList.numStaticDvars)
                {
                    const auto* staticDvar = statement->supportingData->staticDvarList.staticDvars[staticDvarEntry.data.operand.internals.intVal];
                    if (staticDvar && staticDvar->dvarName)
                        m_stream << staticDvar->dvarName;
                }
                else
                {
                    m_stream << "#INVALID_DVAR_INDEX";
                }
            }
            else
            {
                m_stream << "#INVALID_DVAR_OPERAND";
            }
        }

        m_stream << ")";
        currentPos = closingParenPos + 1;
        spaceNext = true;
    }
    else
    {
        if (expEntry.data.op >= 0 && static_cast<unsigned>(expEntry.data.op) < std::extent_v<decltype(g_expFunctionNames)>)
            m_stream << g_expFunctionNames[expEntry.data.op];

        if (expEntry.data.op >= OP_COUNT)
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

        spaceNext = expEntry.data.op != OP_NOT;
    }
}

void MenuDumper::WriteStatementOperandFunction(const Statement_s* statement, size_t currentPos) const
{
    const auto& operand = statement->entries[currentPos].data.operand;

    if (operand.internals.function == nullptr)
        return;

    if (!ObjWriting::Configuration.MenuLegacyMode)
    {
        int functionIndex = -1;
        if (statement->supportingData && statement->supportingData->uifunctions.functions)
        {
            for (auto supportingFunctionIndex = 0; supportingFunctionIndex < statement->supportingData->uifunctions.totalFunctions; supportingFunctionIndex++)
            {
                if (statement->supportingData->uifunctions.functions[supportingFunctionIndex] == operand.internals.function)
                {
                    functionIndex = supportingFunctionIndex;
                    break;
                }
            }
        }

        if (functionIndex >= 0)
            m_stream << "FUNC_" << functionIndex;
        else
            m_stream << "INVALID_FUNC";
    }
    else
    {
        m_stream << "(";
        WriteStatementSkipInitialUnnecessaryParenthesis(operand.internals.function);
        m_stream << ")";
    }
}

void MenuDumper::WriteStatementOperand(const Statement_s* statement, size_t& currentPos, bool& spaceNext) const
{
    const auto& expEntry = statement->entries[currentPos];

    if (spaceNext)
        m_stream << " ";

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
        WriteStatementOperandFunction(statement, currentPos);
        break;

    default:
        break;
    }

    currentPos++;
    spaceNext = true;
}

void MenuDumper::WriteStatementEntryRange(const Statement_s* statement, size_t startOffset, size_t endOffset) const
{
    assert(startOffset <= endOffset);
    assert(endOffset <= static_cast<size_t>(statement->numEntries));

    auto currentPos = startOffset;
    auto spaceNext = false;
    while (currentPos < endOffset)
    {
        const auto& expEntry = statement->entries[currentPos];

        if (expEntry.type == EET_OPERATOR)
        {
            WriteStatementOperator(statement, currentPos, spaceNext);
        }
        else
        {
            WriteStatementOperand(statement, currentPos, spaceNext);
        }
    }
}

void MenuDumper::WriteStatement(const Statement_s* statement) const
{
    if (statement == nullptr || statement->numEntries < 0)
        return;

    WriteStatementEntryRange(statement, 0, static_cast<size_t>(statement->numEntries));
}

void MenuDumper::WriteStatementSkipInitialUnnecessaryParenthesis(const Statement_s* statementValue) const
{
    if (statementValue == nullptr || statementValue->numEntries < 0)
        return;

    const auto statementEnd = static_cast<size_t>(statementValue->numEntries);

    if (statementValue->numEntries >= 1
        && statementValue->entries[0].type == EET_OPERATOR
        && statementValue->entries[0].data.op == OP_LEFTPAREN)
    {
        const auto parenthesisEnd = FindStatementClosingParenthesis(statementValue, 0);

        if (parenthesisEnd >= statementEnd)
            WriteStatementEntryRange(statementValue, 1, statementEnd);
        else if (parenthesisEnd == statementEnd - 1)
            WriteStatementEntryRange(statementValue, 1, statementEnd - 1);
        else
            WriteStatementEntryRange(statementValue, 0, statementEnd);
    }
    else
    {
        WriteStatementEntryRange(statementValue, 0, statementEnd);
    }
}

void MenuDumper::WriteStatementProperty(const std::string& propertyKey, const Statement_s* statementValue, bool isBooleanStatement) const
{
    if (statementValue == nullptr || statementValue->numEntries < 0)
        return;

    Indent();
    WriteKey(propertyKey);

    if (isBooleanStatement)
    {
        m_stream << "when(";
        WriteStatementSkipInitialUnnecessaryParenthesis(statementValue);
        m_stream << ");\n";
    }
    else
    {
        WriteStatementSkipInitialUnnecessaryParenthesis(statementValue);
        m_stream << ";\n";
    }
}

void MenuDumper::WriteSetLocalVarData(const std::string& setFunction, const SetLocalVarData* setLocalVarData) const
{
    if (setLocalVarData == nullptr)
        return;

    Indent();
    m_stream << setFunction << " " << setLocalVarData->localVarName << " ";
    WriteStatement(setLocalVarData->expression);
    m_stream << ";\n";
}

//#define WRITE_ORIGINAL_SCRIPT

void MenuDumper::WriteUnconditionalScript(const char* script) const
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
            m_stream << "\"" << token << "\"";
        else
            m_stream << token;
    }

    if (!isNewStatement)
        m_stream << ";\n";
}

void MenuDumper::WriteMenuEventHandlerSet(const MenuEventHandlerSet* eventHandlerSet)
{
    Indent();
    m_stream << "{\n";
    IncIndent();

    for (auto i = 0; i < eventHandlerSet->eventHandlerCount; i++)
    {
        const auto* eventHandler = eventHandlerSet->eventHandlers[i];
        if (eventHandler == nullptr)
            continue;

        switch (eventHandler->eventType)
        {
        case EVENT_UNCONDITIONAL:
            WriteUnconditionalScript(eventHandler->eventData.unconditionalScript);
            break;

        case EVENT_IF:
            if (eventHandler->eventData.conditionalScript == nullptr
                || eventHandler->eventData.conditionalScript->eventExpression == nullptr
                || eventHandler->eventData.conditionalScript->eventHandlerSet == nullptr)
            {
                continue;
            }

            Indent();
            m_stream << "if (";
            WriteStatementSkipInitialUnnecessaryParenthesis(eventHandler->eventData.conditionalScript->eventExpression);
            m_stream << ")\n";
            WriteMenuEventHandlerSet(eventHandler->eventData.conditionalScript->eventHandlerSet);
            break;

        case EVENT_ELSE:
            if (eventHandler->eventData.elseScript == nullptr)
                continue;

            Indent();
            m_stream << "else\n";
            WriteMenuEventHandlerSet(eventHandler->eventData.elseScript);
            break;

        case EVENT_SET_LOCAL_VAR_BOOL:
            WriteSetLocalVarData("setLocalVarBool", eventHandler->eventData.setLocalVarData);
            break;

        case EVENT_SET_LOCAL_VAR_INT:
            WriteSetLocalVarData("setLocalVarInt", eventHandler->eventData.setLocalVarData);
            break;

        case EVENT_SET_LOCAL_VAR_FLOAT:
            WriteSetLocalVarData("setLocalVarFloat", eventHandler->eventData.setLocalVarData);
            break;

        case EVENT_SET_LOCAL_VAR_STRING:
            WriteSetLocalVarData("setLocalVarString", eventHandler->eventData.setLocalVarData);
            break;

        default:
            break;
        }
    }

    DecIndent();
    Indent();
    m_stream << "}\n";
}

void MenuDumper::WriteMenuEventHandlerSetProperty(const std::string& propertyKey, const MenuEventHandlerSet* eventHandlerSetValue)
{
    if (eventHandlerSetValue == nullptr)
        return;

    Indent();
    m_stream << propertyKey << "\n";
    WriteMenuEventHandlerSet(eventHandlerSetValue);
}

void MenuDumper::WriteRectProperty(const std::string& propertyKey, const rectDef_s& rect) const
{
    Indent();
    WriteKey(propertyKey);
    m_stream << rect.x << " " << rect.y << " " << rect.w << " " << rect.h << " " << static_cast<int>(rect.horzAlign) << " " << static_cast<int>(rect.vertAlign) << "\n";
}

void MenuDumper::WriteMaterialProperty(const std::string& propertyKey, const Material* materialValue) const
{
    if (materialValue == nullptr || materialValue->info.name == nullptr)
        return;

    if (materialValue->info.name[0] == ',')
        WriteStringProperty(propertyKey, &materialValue->info.name[1]);
    else
        WriteStringProperty(propertyKey, materialValue->info.name);
}

void MenuDumper::WriteSoundAliasProperty(const std::string& propertyKey, const snd_alias_list_t* soundAliasValue) const
{
    if (soundAliasValue == nullptr)
        return;

    WriteStringProperty(propertyKey, soundAliasValue->aliasName);
}

void MenuDumper::WriteDecodeEffectProperty(const std::string& propertyKey, const itemDef_s* item) const
{
    if (!item->decayActive)
        return;

    Indent();
    WriteKey(propertyKey);
    m_stream << item->fxLetterTime << " " << item->fxDecayStartTime << " " << item->fxDecayDuration << "\n";
}

void MenuDumper::WriteItemKeyHandlerProperty(const ItemKeyHandler* itemKeyHandlerValue)
{
    for (const auto* currentHandler = itemKeyHandlerValue; currentHandler; currentHandler = currentHandler->next)
    {
        if (currentHandler->key >= '!' && currentHandler->key <= '~' && currentHandler->key != '"')
        {
            std::ostringstream ss;
            ss << "execKey \"" << static_cast<char>(currentHandler->key) << "\"";
            WriteMenuEventHandlerSetProperty(ss.str(), currentHandler->action);
        }
        else
        {
            std::ostringstream ss;
            ss << "execKeyInt " << currentHandler->key;
            WriteMenuEventHandlerSetProperty(ss.str(), currentHandler->action);
        }
    }
}

void MenuDumper::WriteFloatExpressionsProperty(const ItemFloatExpression* floatExpressions, int floatExpressionCount) const
{
    if (!floatExpressions)
        return;

    for (int i = 0; i < floatExpressionCount; i++)
    {
        const auto& floatExpression = floatExpressions[i];

        if (floatExpression.target < 0 || floatExpression.target >= ITEM_FLOATEXP_TGT_COUNT)
            continue;

        std::string propertyName = std::string("exp ") + floatExpressionTargetBindings[floatExpression.target].name + std::string(" ")
            + floatExpressionTargetBindings[floatExpression.target].componentName;

        WriteStatementProperty(propertyName, floatExpression.expression, false);
    }
}

void MenuDumper::WriteColumnProperty(const std::string& propertyKey, const listBoxDef_s* listBox) const
{
    if (listBox->numColumns <= 0)
        return;

    Indent();
    WriteKey(propertyKey);
    m_stream << listBox->numColumns << "\n";

    for (auto col = 0; col < listBox->numColumns; col++)
    {
        Indent();
        for (auto i = 0u; i < MENU_KEY_SPACING; i++)
            m_stream << " ";

        m_stream << listBox->columnInfo[col].xpos
            << " " << listBox->columnInfo[col].ypos
            << " " << listBox->columnInfo[col].width
            << " " << listBox->columnInfo[col].height
            << " " << listBox->columnInfo[col].maxChars
            << " " << listBox->columnInfo[col].alignment << "\n";
    }
}

void MenuDumper::WriteListBoxProperties(const itemDef_s* item)
{
    if (item->type != ITEM_TYPE_LISTBOX || item->typeData.listBox == nullptr)
        return;

    const auto* listBox = item->typeData.listBox;
    WriteKeywordProperty("notselectable", listBox->notselectable != 0);
    WriteKeywordProperty("noscrollbars", listBox->noScrollBars != 0);
    WriteKeywordProperty("usepaging", listBox->usePaging != 0);
    WriteFloatProperty("elementwidth", listBox->elementWidth, 0.0f);
    WriteFloatProperty("elementheight", listBox->elementHeight, 0.0f);
    WriteFloatProperty("feeder", item->special, 0.0f);
    WriteIntProperty("elementtype", listBox->elementStyle, 0);
    WriteColumnProperty("columns", listBox);
    WriteMenuEventHandlerSetProperty("doubleclick", listBox->onDoubleClick);
    WriteColorProperty("selectBorder", listBox->selectBorder, COLOR_0000);
    WriteMaterialProperty("selectIcon", listBox->selectIcon);
}

void MenuDumper::WriteDvarFloatProperty(const std::string& propertyKey, const itemDef_s* item, const editFieldDef_s* editField) const
{
    if (item->dvar == nullptr)
        return;

    Indent();
    WriteKey(propertyKey);
    m_stream << "\"" << item->dvar << "\" " << editField->stepVal << " " << editField->minVal << " " << editField->maxVal << "\n";
}

void MenuDumper::WriteEditFieldProperties(const itemDef_s* item) const
{
    switch (item->type)
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
    case ITEM_TYPE_EMAILFIELD:
    case ITEM_TYPE_PASSWORDFIELD:
        break;

    default:
        return;
    }

    if (item->typeData.editField == nullptr)
        return;

    const auto* editField = item->typeData.editField;
    if (std::fabs(-1.0f - editField->stepVal) >= std::numeric_limits<float>::epsilon()
        || std::fabs(-1.0f - editField->minVal) >= std::numeric_limits<float>::epsilon()
        || std::fabs(-1.0f - editField->maxVal) >= std::numeric_limits<float>::epsilon())
    {
        WriteDvarFloatProperty("dvarFloat", item, editField);
    }
    else
    {
        WriteStringProperty("dvar", item->dvar);
    }
    WriteStringProperty("localvar", item->localVar);
    WriteIntProperty("maxChars", editField->maxChars, 0);
    WriteKeywordProperty("maxCharsGotoNext", editField->maxCharsGotoNext != 0);
    WriteIntProperty("maxPaintChars", editField->maxPaintChars, 0);
}

void MenuDumper::WriteMultiValueProperty(const multiDef_s* multiDef) const
{
    Indent();
    if (multiDef->strDef)
        WriteKey("dvarStrList");
    else
        WriteKey("dvarFloatList");

    m_stream << "{";
    for (auto i = 0; i < multiDef->count; i++)
    {
        if (multiDef->dvarList[i] == nullptr || multiDef->strDef && multiDef->dvarStr[i] == nullptr)
            continue;

        m_stream << " \"" << multiDef->dvarList[i] << "\"";

        if (multiDef->strDef)
            m_stream << " \"" << multiDef->dvarStr[i] << "\"";
        else
            m_stream << " " << multiDef->dvarValue[i] << "";
    }
    m_stream << " }\n";
}

void MenuDumper::WriteMultiProperties(const itemDef_s* item) const
{
    if (item->type != ITEM_TYPE_MULTI || item->typeData.multi == nullptr)
        return;

    const auto* multiDef = item->typeData.multi;

    if (multiDef->count <= 0)
        return;

    WriteStringProperty("dvar", item->dvar);
    WriteMultiValueProperty(multiDef);
}

void MenuDumper::WriteEnumDvarProperties(const itemDef_s* item) const
{
    if (item->type != ITEM_TYPE_DVARENUM)
        return;

    WriteStringProperty("dvarEnumList", item->typeData.enumDvarName);
}

void MenuDumper::WriteTickerProperties(const itemDef_s* item) const
{
    if (item->type != ITEM_TYPE_NEWS_TICKER || item->typeData.ticker == nullptr)
        return;

    const auto* newsTickerDef = item->typeData.ticker;
    WriteIntProperty("spacing", newsTickerDef->spacing, 0);
    WriteIntProperty("speed", newsTickerDef->speed, 0);
    WriteIntProperty("newsfeed", newsTickerDef->feedId, 0);
}

void MenuDumper::WriteItemData(const itemDef_s* item)
{
    WriteStringProperty("name", item->window.name);
    WriteStringProperty("text", item->text);
    WriteKeywordProperty("textsavegame", item->itemFlags & ITEM_FLAG_SAVE_GAME_INFO);
    WriteKeywordProperty("textcinematicsubtitle", item->itemFlags & ITEM_FLAG_CINEMATIC_SUBTITLE);
    WriteStringProperty("group", item->window.group);
    WriteRectProperty("rect", item->window.rect);
    WriteIntProperty("style", item->window.style, 0);
    WriteKeywordProperty("decoration", item->window.staticFlags & WINDOW_FLAG_DECORATION);
    WriteKeywordProperty("autowrapped", item->window.staticFlags & WINDOW_FLAG_AUTO_WRAPPED);
    WriteKeywordProperty("horizontalscroll", item->window.staticFlags & WINDOW_FLAG_HORIZONTAL_SCROLL);
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
    WriteColorProperty("glowcolor", item->glowColor, COLOR_0000);
    WriteMaterialProperty("background", item->window.background);
    WriteMenuEventHandlerSetProperty("onFocus", item->onFocus);
    WriteMenuEventHandlerSetProperty("leaveFocus", item->leaveFocus);
    WriteMenuEventHandlerSetProperty("mouseEnter", item->mouseEnter);
    WriteMenuEventHandlerSetProperty("mouseExit", item->mouseExit);
    WriteMenuEventHandlerSetProperty("mouseEnterText", item->mouseEnterText);
    WriteMenuEventHandlerSetProperty("mouseExitText", item->mouseExitText);
    WriteMenuEventHandlerSetProperty("action", item->action);
    WriteMenuEventHandlerSetProperty("accept", item->accept);
    // WriteFloatProperty("special", item->special, 0.0f);
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
    WriteFloatExpressionsProperty(item->floatExpressions, item->floatExpressionCount);
    WriteIntProperty("gamemsgwindowindex", item->gameMsgWindowIndex, 0);
    WriteIntProperty("gamemsgwindowmode", item->gameMsgWindowMode, 0);
    WriteDecodeEffectProperty("decodeEffect", item);

    WriteListBoxProperties(item);
    WriteEditFieldProperties(item);
    WriteMultiProperties(item);
    WriteEnumDvarProperties(item);
    WriteTickerProperties(item);
}

void MenuDumper::WriteItemDefs(const itemDef_s* const* itemDefs, size_t itemCount)
{
    for (auto i = 0u; i < itemCount; i++)
    {
        StartItemDefScope();

        WriteItemData(itemDefs[i]);

        EndScope();
    }
}

void MenuDumper::WriteMenuData(const menuDef_t* menu)
{
    WriteStringProperty("name", menu->window.name);
    WriteBoolProperty("fullscreen", menu->data->fullScreen, false);
    WriteKeywordProperty("screenSpace", menu->window.staticFlags & WINDOW_FLAG_SCREEN_SPACE);
    WriteKeywordProperty("decoration", menu->window.staticFlags & WINDOW_FLAG_DECORATION);
    WriteRectProperty("rect", menu->window.rect);
    WriteIntProperty("style", menu->window.style, 0);
    WriteIntProperty("border", menu->window.border, 0);
    WriteFloatProperty("borderSize", menu->window.borderSize, 0.0f);
    WriteColorProperty("backcolor", menu->window.backColor, COLOR_0000);
    WriteColorProperty("forecolor", menu->window.foreColor, COLOR_1111);
    WriteColorProperty("bordercolor", menu->window.borderColor, COLOR_0000);
    WriteColorProperty("focuscolor", menu->data->focusColor, COLOR_0000);
    WriteMaterialProperty("background", menu->window.background);
    WriteIntProperty("ownerdraw", menu->window.ownerDraw, 0);
    WriteFlagsProperty("ownerdrawFlag", menu->window.ownerDrawFlags);
    WriteKeywordProperty("outOfBoundsClick", menu->window.staticFlags & WINDOW_FLAG_OUT_OF_BOUNDS_CLICK);
    WriteStringProperty("soundLoop", menu->data->soundName);
    WriteKeywordProperty("popup", menu->window.staticFlags & WINDOW_FLAG_POPUP);
    WriteFloatProperty("fadeClamp", menu->data->fadeClamp, 0.0f);
    WriteIntProperty("fadeCycle", menu->data->fadeCycle, 0);
    WriteFloatProperty("fadeAmount", menu->data->fadeAmount, 0.0f);
    WriteFloatProperty("fadeInAmount", menu->data->fadeInAmount, 0.0f);
    WriteFloatProperty("blurWorld", menu->data->blurRadius, 0.0f);
    WriteKeywordProperty("legacySplitScreenScale", menu->window.staticFlags & WINDOW_FLAG_LEGACY_SPLIT_SCREEN_SCALE);
    WriteKeywordProperty("hiddenDuringScope", menu->window.staticFlags & WINDOW_FLAG_HIDDEN_DURING_SCOPE);
    WriteKeywordProperty("hiddenDuringFlashbang", menu->window.staticFlags & WINDOW_FLAG_HIDDEN_DURING_FLASH_BANG);
    WriteKeywordProperty("hiddenDuringUI", menu->window.staticFlags & WINDOW_FLAG_HIDDEN_DURING_UI);
    WriteStringProperty("allowedBinding", menu->data->allowedBinding);
    WriteKeywordProperty("textOnlyFocus", menu->window.staticFlags & WINDOW_FLAG_TEXT_ONLY_FOCUS);
    WriteStatementProperty("visible", menu->data->visibleExp, true);
    WriteStatementProperty("exp rect X", menu->data->rectXExp, false);
    WriteStatementProperty("exp rect Y", menu->data->rectYExp, false);
    WriteStatementProperty("exp rect W", menu->data->rectWExp, false);
    WriteStatementProperty("exp rect H", menu->data->rectHExp, false);
    WriteStatementProperty("exp openSound", menu->data->openSoundExp, false);
    WriteStatementProperty("exp closeSound", menu->data->closeSoundExp, false);
    WriteMenuEventHandlerSetProperty("onOpen", menu->data->onOpen);
    WriteMenuEventHandlerSetProperty("onClose", menu->data->onClose);
    WriteMenuEventHandlerSetProperty("onRequestClose", menu->data->onCloseRequest);
    WriteMenuEventHandlerSetProperty("onESC", menu->data->onESC);
    WriteItemKeyHandlerProperty(menu->data->onKey);
    WriteItemDefs(menu->items, menu->itemCount);
}

MenuDumper::MenuDumper(std::ostream& stream)
    : AbstractMenuDumper(stream)
{
}

void MenuDumper::WriteFunctionDef(const std::string& functionName, const Statement_s* statement)
{
    StartFunctionDefScope();

    WriteStringProperty("name", functionName);
    WriteStatementProperty("value", statement, false);

    EndScope();
}

void MenuDumper::WriteMenu(const menuDef_t* menu)
{
    StartMenuDefScope();

    WriteMenuData(menu);

    EndScope();
}
