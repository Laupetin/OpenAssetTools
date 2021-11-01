#include "SequenceMenuDef.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

using namespace menu;

SequenceMenuDef::SequenceMenuDef()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("menuDef"),
        create.Char('{'),
    });
}

void SequenceMenuDef::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    assert(!state->m_current_menu);

    state->m_current_menu = std::make_unique<CommonMenuDef>();
}
