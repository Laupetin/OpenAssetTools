#include "GenericIntPropertySequence.h"

#include <utility>

#include "Parsing/Menu/MenuMatcherFactory.h"

using namespace menu;

GenericIntPropertySequence::GenericIntPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.KeywordIgnoreCase(std::move(keywordName)),
        create.Integer().Capture(CAPTURE_VALUE)
    });
}

void GenericIntPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        const auto value = result.NextCapture(CAPTURE_VALUE).IntegerValue();
        m_set_callback(state, value);
    }
}
