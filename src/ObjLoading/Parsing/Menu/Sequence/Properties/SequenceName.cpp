#include "SequenceName.h"

#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

SequenceName::SequenceName()
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.KeywordIgnoreCase("name"),
        create.Text().Capture(CAPTURE_NAME)
    });
}

void SequenceName::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    assert(state->m_current_item || state->m_current_menu || state->m_current_function);
    
    const auto nameValue = MenuMatcherFactory::TokenTextValue(result.NextCapture(CAPTURE_NAME));

    if (state->m_current_item)
        state->m_current_item->m_name = nameValue;
    else if (state->m_current_menu)
        state->m_current_menu->m_name = nameValue;
    else if (state->m_current_function)
        state->m_current_function->m_name = nameValue;
}
