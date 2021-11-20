#include "GenericKeywordPropertySequence.h"

#include <utility>

#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

GenericKeywordPropertySequence::GenericKeywordPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddMatchers({
        create.KeywordIgnoreCase(std::move(keywordName)).Capture(CAPTURE_FIRST_TOKEN),
    });
}

void GenericKeywordPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        m_set_callback(state, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos());
    }
}
