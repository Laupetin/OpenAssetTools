#include "GenericStringPropertySequence.h"

#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

#include <utility>

using namespace menu;

GenericStringPropertySequence::GenericStringPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddMatchers({create.KeywordIgnoreCase(std::move(keywordName)).Capture(CAPTURE_FIRST_TOKEN), create.Text().Capture(CAPTURE_VALUE)});
}

void GenericStringPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        const auto& value = MenuMatcherFactory::TokenTextValue(result.NextCapture(CAPTURE_VALUE));
        m_set_callback(state, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), value);
    }
}
