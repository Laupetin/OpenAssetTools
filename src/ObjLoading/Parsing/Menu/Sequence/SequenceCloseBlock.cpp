#include "SequenceCloseBlock.h"

#include <sstream>

#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

SequenceCloseBlock::SequenceCloseBlock()
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.Char('}').Capture(CAPTURE_TOKEN)
    });
}

void SequenceCloseBlock::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    assert(state->m_current_item || state->m_current_menu || state->m_current_function || state->m_in_global_scope);
    assert(!state->m_current_item || (state->m_current_item && state->m_current_menu));

    if(state->m_current_item && state->m_current_menu)
    {
        state->m_current_menu->m_items.emplace_back(std::move(state->m_current_item));
        state->m_current_item = nullptr;
    }
    else if(state->m_current_menu)
    {
        if(state->m_current_menu->m_name.empty())
            throw ParsingException(result.NextCapture(CAPTURE_TOKEN).GetPos(), "Menu must have a name");

        const auto existingMenu = state->m_menus_by_name.find(state->m_current_menu->m_name);
        if(existingMenu == state->m_menus_by_name.end())
        {
            state->m_menus_by_name.emplace(std::make_pair(state->m_current_menu->m_name, state->m_current_menu.get()));
            state->m_menus.emplace_back(std::move(state->m_current_menu));
            state->m_current_menu = nullptr;
        }
        else
        {
            std::ostringstream ss;
            ss << "Menu with name \"" << state->m_current_menu->m_name << "\" already exists";
            throw ParsingException(result.NextCapture(CAPTURE_TOKEN).GetPos(), ss.str());
        }
    }
    else if(state->m_current_function)
    {
        const auto existingFunction = state->m_functions_by_name.find(state->m_current_function->m_name);
        if (existingFunction == state->m_functions_by_name.end())
        {
            state->m_functions_by_name.emplace(std::make_pair(state->m_current_function->m_name, state->m_current_function.get()));
            state->m_functions.emplace_back(std::move(state->m_current_function));
            state->m_current_function = nullptr;
        }
        else
        {
            std::ostringstream ss;
            ss << "Function with name \"" << state->m_current_menu->m_name << "\" already exists";
            throw ParsingException(result.NextCapture(CAPTURE_TOKEN).GetPos(), ss.str());
        }
    }
    else if(state->m_in_global_scope)
    {
        state->m_in_global_scope = false;
    }
    else
        throw ParsingException(result.NextCapture(CAPTURE_TOKEN).GetPos(), "Invalid close block");
}
