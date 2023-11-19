#include "GenericBoolPropertySequence.h"

#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

#include <utility>

using namespace menu;

GenericBoolPropertySequence::GenericBoolPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddMatchers({create.KeywordIgnoreCase(std::move(keywordName)).Capture(CAPTURE_FIRST_TOKEN), create.Integer().Capture(CAPTURE_VALUE)});
}

void GenericBoolPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        const auto value = result.NextCapture(CAPTURE_VALUE).IntegerValue();
        m_set_callback(state, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), value > 0);
    }
}
