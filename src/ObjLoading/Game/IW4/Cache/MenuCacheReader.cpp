#include "MenuCacheReader.h"

using namespace IW4;

MenuCacheReader::MenuCacheReader(std::istream& stream, MemoryManager& memory, ExpressionSupportingData* supportingData)
    : AbstractCacheReader(stream, memory),
      m_supporting_data(supportingData)
{
}

Statement_s* MenuCacheReader::ReadStatement() const
{
    auto* statement = static_cast<Statement_s*>(m_memory.Alloc(sizeof(Statement_s)));
    Read(statement, sizeof(Statement_s));

    if (statement->entries && statement->numEntries > 0)
    {
        statement->entries = static_cast<expressionEntry*>(m_memory.Alloc(sizeof(expressionEntry) * statement->numEntries));
        Read(statement->entries, sizeof(expressionEntry) * statement->numEntries);

        for (auto i = 0; i < statement->numEntries; i++)
        {
            auto& entry = statement->entries[i];
            if (entry.type == EET_OPERAND)
            {
                if (entry.data.operand.dataType == VAL_STRING)
                {
                    if (entry.data.operand.internals.stringVal.string)
                        entry.data.operand.internals.stringVal.string = ReadString();
                }
                else if (entry.data.operand.dataType == VAL_FUNCTION)
                {
                    if (entry.data.operand.internals.function)
                        entry.data.operand.internals.function = ReadStatement();
                }
            }
        }
    }

    statement->supportingData = m_supporting_data;

    return statement;
}

ItemKeyHandler* MenuCacheReader::ReadItemKeyHandler() const
{
    auto* itemKeyHandler = static_cast<ItemKeyHandler*>(m_memory.Alloc(sizeof(ItemKeyHandler)));
    Read(itemKeyHandler, sizeof(ItemKeyHandler));

    if (itemKeyHandler->action)
        itemKeyHandler->action = ReadMenuEventHandlerSet();

    if (itemKeyHandler->next)
        itemKeyHandler->next = ReadItemKeyHandler();

    return itemKeyHandler;
}

MenuEventHandler* MenuCacheReader::ReadMenuEventHandler() const
{
    auto* menuEventHandler = static_cast<MenuEventHandler*>(m_memory.Alloc(sizeof(MenuEventHandler)));
    Read(menuEventHandler, sizeof(MenuEventHandler));

    switch (menuEventHandler->eventType)
    {
    case EVENT_UNCONDITIONAL:
        if (menuEventHandler->eventData.unconditionalScript)
            menuEventHandler->eventData.unconditionalScript = ReadString();
        break;

    case EVENT_IF:
        if (menuEventHandler->eventData.conditionalScript)
        {
            auto* conditionalScript = static_cast<ConditionalScript*>(m_memory.Alloc(sizeof(ConditionalScript)));
            if (ReadBool())
                conditionalScript->eventHandlerSet = ReadMenuEventHandlerSet();
            else
                conditionalScript->eventHandlerSet = nullptr;

            if (ReadBool())
                conditionalScript->eventExpression = ReadStatement();
            else
                conditionalScript->eventExpression = nullptr;

            menuEventHandler->eventData.conditionalScript = conditionalScript;
        }
        break;

    case EVENT_ELSE:
        if (menuEventHandler->eventData.elseScript)
            menuEventHandler->eventData.elseScript = ReadMenuEventHandlerSet();
        break;

    case EVENT_SET_LOCAL_VAR_BOOL:
    case EVENT_SET_LOCAL_VAR_FLOAT:
    case EVENT_SET_LOCAL_VAR_INT:
    case EVENT_SET_LOCAL_VAR_STRING:
        if (menuEventHandler->eventData.setLocalVarData)
        {
            auto* setLocalVarData = static_cast<SetLocalVarData*>(m_memory.Alloc(sizeof(SetLocalVarData)));
            if (ReadBool())
                setLocalVarData->localVarName = ReadString();

            if (ReadBool())
                setLocalVarData->expression = ReadStatement();
            else
                setLocalVarData->expression = nullptr;
            menuEventHandler->eventData.setLocalVarData = setLocalVarData;
        }
        break;

    default:
        break;
    }

    return menuEventHandler;
}

MenuEventHandlerSet* MenuCacheReader::ReadMenuEventHandlerSet() const
{
    auto* menuEventHandlerSet = static_cast<MenuEventHandlerSet*>(m_memory.Alloc(sizeof(MenuEventHandlerSet)));
    Read(menuEventHandlerSet, sizeof(MenuEventHandlerSet));

    if (menuEventHandlerSet->eventHandlers)
    {
        menuEventHandlerSet->eventHandlers =
            static_cast<MenuEventHandler**>(m_memory.Alloc(sizeof(MenuEventHandler*) * menuEventHandlerSet->eventHandlerCount));
        for (auto i = 0; i < menuEventHandlerSet->eventHandlerCount; i++)
        {
            if (ReadBool())
                menuEventHandlerSet->eventHandlers[i] = ReadMenuEventHandler();
            else
                menuEventHandlerSet->eventHandlers[i] = nullptr;
        }
    }

    return menuEventHandlerSet;
}

void MenuCacheReader::ReadWindowDef(windowDef_t& windowDef) const
{
    if (windowDef.name)
        windowDef.name = ReadString();

    if (windowDef.group)
        windowDef.group = ReadString();

    // background
}

itemDef_s* MenuCacheReader::ReadItem() const
{
    auto* item = static_cast<itemDef_s*>(m_memory.Alloc(sizeof(itemDef_s)));
    Read(item, sizeof(itemDef_s));

    ReadWindowDef(item->window);

    if (item->text)
        item->text = ReadString();

    if (item->mouseEnterText)
        item->mouseEnterText = ReadMenuEventHandlerSet();

    if (item->mouseExitText)
        item->mouseExitText = ReadMenuEventHandlerSet();

    if (item->mouseEnter)
        item->mouseEnter = ReadMenuEventHandlerSet();

    if (item->mouseExit)
        item->mouseExit = ReadMenuEventHandlerSet();

    if (item->action)
        item->action = ReadMenuEventHandlerSet();

    if (item->accept)
        item->accept = ReadMenuEventHandlerSet();

    if (item->onFocus)
        item->onFocus = ReadMenuEventHandlerSet();

    if (item->leaveFocus)
        item->leaveFocus = ReadMenuEventHandlerSet();

    if (item->dvar)
        item->dvar = ReadString();

    if (item->dvarTest)
        item->dvarTest = ReadString();

    if (item->onKey)
        item->onKey = ReadItemKeyHandler();

    if (item->enableDvar)
        item->enableDvar = ReadString();

    if (item->localVar)
        item->localVar = ReadString();

    // focusSound

    // itemDefData

    // floatExpressions

    if (item->visibleExp)
        item->visibleExp = ReadStatement();

    if (item->disabledExp)
        item->disabledExp = ReadStatement();

    if (item->textExp)
        item->textExp = ReadStatement();

    if (item->materialExp)
        item->materialExp = ReadStatement();

    return item;
}

menuDef_t* MenuCacheReader::ReadMenu() const
{
    auto* menu = static_cast<menuDef_t*>(m_memory.Alloc(sizeof(menuDef_t)));
    Read(menu, sizeof(menuDef_t));

    menu->expressionData = m_supporting_data;

    ReadWindowDef(menu->window);

    if (menu->font)
        menu->font = ReadString();

    if (menu->onOpen)
        menu->onOpen = ReadMenuEventHandlerSet();

    if (menu->onClose)
        menu->onClose = ReadMenuEventHandlerSet();

    if (menu->onCloseRequest)
        menu->onCloseRequest = ReadMenuEventHandlerSet();

    if (menu->onESC)
        menu->onESC = ReadMenuEventHandlerSet();

    if (menu->onKey)
        menu->onKey = ReadItemKeyHandler();

    if (menu->visibleExp)
        menu->visibleExp = ReadStatement();

    if (menu->allowedBinding)
        menu->allowedBinding = ReadString();

    if (menu->soundName)
        menu->soundName = ReadString();

    if (menu->rectXExp)
        menu->rectXExp = ReadStatement();

    if (menu->rectYExp)
        menu->rectYExp = ReadStatement();

    if (menu->rectWExp)
        menu->rectWExp = ReadStatement();

    if (menu->rectHExp)
        menu->rectHExp = ReadStatement();

    if (menu->openSoundExp)
        menu->openSoundExp = ReadStatement();

    if (menu->closeSoundExp)
        menu->closeSoundExp = ReadStatement();

    if (menu->items)
    {
        menu->items = static_cast<itemDef_s**>(m_memory.Alloc(sizeof(itemDef_s*) * menu->itemCount));
        for (auto i = 0; i < menu->itemCount; i++)
        {
            if (ReadBool())
                menu->items[i] = ReadItem();
            else
                menu->items[i] = nullptr;
        }
    }

    return menu;
}
