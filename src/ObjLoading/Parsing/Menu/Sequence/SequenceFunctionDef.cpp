#include "SequenceFunctionDef.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceFunctionDef::SequenceFunctionDef()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("functionDef"),
        create.Char('{'),
    });
}

void SequenceFunctionDef::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    assert(!state->m_current_menu);

    state->m_current_item = std::make_unique<CommonItemDef>();
}
