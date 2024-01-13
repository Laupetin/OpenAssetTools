#include "MenuCacheWriter.h"

using namespace IW4;

MenuCacheWriter::MenuCacheWriter(std::ostream& stream)
    : AbstractCacheWriter(stream)
{
}

void MenuCacheWriter::WriteItemKeyHandler(const ItemKeyHandler& itemKeyHandler) const
{
    Write(&itemKeyHandler, sizeof(ItemKeyHandler));

    if (itemKeyHandler.action)
        WriteMenuEventHandlerSet(*itemKeyHandler.action);

    if (itemKeyHandler.next)
        WriteItemKeyHandler(*itemKeyHandler.next);
}

void MenuCacheWriter::WriteStatement(const Statement_s& statement) const
{
    Write(&statement, sizeof(Statement_s));

    if (statement.entries && statement.numEntries > 0)
    {
        Write(statement.entries, sizeof(expressionEntry) * statement.numEntries);

        for (auto i = 0; i < statement.numEntries; i++)
        {
            const auto& entry = statement.entries[i];
            if (entry.type == EET_OPERAND)
            {
                if (entry.data.operand.dataType == VAL_STRING)
                {
                    WriteString(entry.data.operand.internals.stringVal.string);
                }
                else if (entry.data.operand.dataType == VAL_FUNCTION)
                {
                    if (entry.data.operand.internals.function)
                        WriteStatement(*entry.data.operand.internals.function);
                }
            }
        }
    }

    // supportingData
}

void MenuCacheWriter::WriteMenuEventHandler(const MenuEventHandler& menuEventHandler) const
{
    Write(&menuEventHandler, sizeof(MenuEventHandler));

    switch (menuEventHandler.eventType)
    {
    case EVENT_UNCONDITIONAL:
        WriteString(menuEventHandler.eventData.unconditionalScript);
        break;

    case EVENT_IF:
        if (menuEventHandler.eventData.conditionalScript)
        {
            if (menuEventHandler.eventData.conditionalScript->eventHandlerSet)
            {
                WriteTrue();
                WriteMenuEventHandlerSet(*menuEventHandler.eventData.conditionalScript->eventHandlerSet);
            }
            else
                WriteFalse();

            if (menuEventHandler.eventData.conditionalScript->eventExpression)
            {
                WriteTrue();
                WriteStatement(*menuEventHandler.eventData.conditionalScript->eventExpression);
            }
            else
                WriteFalse();
        }
        break;

    case EVENT_ELSE:
        if (menuEventHandler.eventData.elseScript)
            WriteMenuEventHandlerSet(*menuEventHandler.eventData.elseScript);
        break;

    case EVENT_SET_LOCAL_VAR_BOOL:
    case EVENT_SET_LOCAL_VAR_FLOAT:
    case EVENT_SET_LOCAL_VAR_INT:
    case EVENT_SET_LOCAL_VAR_STRING:
        if (menuEventHandler.eventData.setLocalVarData)
        {
            WriteBool(menuEventHandler.eventData.setLocalVarData->localVarName != nullptr);
            WriteString(menuEventHandler.eventData.setLocalVarData->localVarName);

            if (menuEventHandler.eventData.setLocalVarData->expression)
            {
                WriteTrue();
                WriteStatement(*menuEventHandler.eventData.setLocalVarData->expression);
            }
            else
                WriteFalse();
        }
        break;

    default:
        break;
    }
}

void MenuCacheWriter::WriteMenuEventHandlerSet(const MenuEventHandlerSet& menuEventHandlerSet) const
{
    Write(&menuEventHandlerSet, sizeof(MenuEventHandlerSet));

    if (menuEventHandlerSet.eventHandlers)
    {
        for (auto i = 0; i < menuEventHandlerSet.eventHandlerCount; i++)
        {
            if (menuEventHandlerSet.eventHandlers[i])
            {
                WriteTrue();
                WriteMenuEventHandler(*menuEventHandlerSet.eventHandlers[i]);
            }
            else
                WriteFalse();
        }
    }
}

void MenuCacheWriter::WriteWindowDef(const windowDef_t& windowDef) const
{
    WriteString(windowDef.name);

    WriteString(windowDef.group);

    // background
}

void MenuCacheWriter::WriteItem(const itemDef_s& item) const
{
    Write(&item, sizeof(item));

    WriteWindowDef(item.window);

    WriteString(item.text);

    if (item.mouseEnterText)
        WriteMenuEventHandlerSet(*item.mouseEnterText);

    if (item.mouseExitText)
        WriteMenuEventHandlerSet(*item.mouseExitText);

    if (item.mouseEnter)
        WriteMenuEventHandlerSet(*item.mouseEnter);

    if (item.mouseExit)
        WriteMenuEventHandlerSet(*item.mouseExit);

    if (item.action)
        WriteMenuEventHandlerSet(*item.action);

    if (item.accept)
        WriteMenuEventHandlerSet(*item.accept);

    if (item.onFocus)
        WriteMenuEventHandlerSet(*item.onFocus);

    if (item.leaveFocus)
        WriteMenuEventHandlerSet(*item.leaveFocus);

    WriteString(item.dvar);

    WriteString(item.dvarTest);

    if (item.onKey)
        WriteItemKeyHandler(*item.onKey);

    WriteString(item.enableDvar);

    WriteString(item.localVar);

    // focusSound

    // itemDefData

    // floatExpressions

    if (item.visibleExp)
        WriteStatement(*item.visibleExp);

    if (item.disabledExp)
        WriteStatement(*item.disabledExp);

    if (item.textExp)
        WriteStatement(*item.textExp);

    if (item.materialExp)
        WriteStatement(*item.materialExp);
}

void MenuCacheWriter::WriteMenu(const menuDef_t& menu) const
{
    Write(&menu, sizeof(menu));

    // expressionData

    WriteWindowDef(menu.window);

    WriteString(menu.font);

    if (menu.onOpen)
        WriteMenuEventHandlerSet(*menu.onOpen);

    if (menu.onClose)
        WriteMenuEventHandlerSet(*menu.onClose);

    if (menu.onCloseRequest)
        WriteMenuEventHandlerSet(*menu.onCloseRequest);

    if (menu.onESC)
        WriteMenuEventHandlerSet(*menu.onESC);

    if (menu.onKey)
        WriteItemKeyHandler(*menu.onKey);

    if (menu.visibleExp)
        WriteStatement(*menu.visibleExp);

    WriteString(menu.allowedBinding);

    WriteString(menu.soundName);

    if (menu.rectXExp)
        WriteStatement(*menu.rectXExp);

    if (menu.rectYExp)
        WriteStatement(*menu.rectYExp);

    if (menu.rectWExp)
        WriteStatement(*menu.rectWExp);

    if (menu.rectHExp)
        WriteStatement(*menu.rectHExp);

    if (menu.openSoundExp)
        WriteStatement(*menu.openSoundExp);

    if (menu.closeSoundExp)
        WriteStatement(*menu.closeSoundExp);

    if (menu.items)
    {
        for (auto i = 0; i < menu.itemCount; i++)
        {
            if (menu.items[i])
            {
                WriteTrue();
                WriteItem(*menu.items[i]);
            }
            else
                WriteFalse();
        }
    }
}
