#include "GenericMenuEventHandlerSetPropertySequence.h"

#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

#include <utility>

using namespace menu;

GenericMenuEventHandlerSetPropertySequence::GenericMenuEventHandlerSetPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.KeywordIgnoreCase(std::move(keywordName)).Capture(CAPTURE_FIRST_TOKEN),
        create.Char('{'),
    });
}

void GenericMenuEventHandlerSetPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        auto& eventHandlerPtr = m_set_callback(state, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos());
        if (!eventHandlerPtr)
            eventHandlerPtr = std::make_unique<CommonEventHandlerSet>();

        state->m_current_event_handler_set = eventHandlerPtr.get();
        state->m_current_nested_event_handler_set = eventHandlerPtr.get();
    }
}
