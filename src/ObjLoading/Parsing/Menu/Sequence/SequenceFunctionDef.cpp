#include "SequenceFunctionDef.h"

#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

SequenceFunctionDef::SequenceFunctionDef()
{
    const MenuMatcherFactory create(this);

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
