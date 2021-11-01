#include "SequenceOpenGlobalScopeBlock.h"

#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

SequenceOpenGlobalScopeBlock::SequenceOpenGlobalScopeBlock()
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.Char('{')
    });
}

void SequenceOpenGlobalScopeBlock::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    assert(!state->m_in_global_scope);
    state->m_in_global_scope = true;
}
