#include "GenericMenuEventHandlerSetPropertySequence.h"

#include <utility>

#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

GenericMenuEventHandlerSetPropertySequence::GenericMenuEventHandlerSetPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.KeywordIgnoreCase(std::move(keywordName)),
        create.Char('{')
    });
}

void GenericMenuEventHandlerSetPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_current_event_handler_set = std::make_unique<CommonEventHandlerSet>();
    state->m_event_handler_set_callback = m_set_callback;
}
