#include "SequenceItemDef.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

using namespace menu;

SequenceItemDef::SequenceItemDef()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("itemDef"),
        create.Char('{'),
    });
}

void SequenceItemDef::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    assert(state->m_current_menu);

    state->m_current_item = std::make_unique<CommonItemDef>();
}
