#include "SequenceName.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceName::SequenceName()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.KeywordIgnoreCase("name"),
        create.Or({create.String(), create.Identifier()}).Capture(CAPTURE_NAME)
    });
}

void SequenceName::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    assert(state->m_current_item || state->m_current_menu || state->m_current_function);

    const auto& nameToken = result.NextCapture(CAPTURE_NAME);
    const auto nameValue = nameToken.m_type == SimpleParserValueType::IDENTIFIER ? nameToken.IdentifierValue() : nameToken.StringValue();

    if (state->m_current_item)
        state->m_current_item->m_name = nameValue;
    else if (state->m_current_menu)
        state->m_current_menu->m_name = nameValue;
    else if (state->m_current_function)
        state->m_current_function->m_name = nameValue;
}
