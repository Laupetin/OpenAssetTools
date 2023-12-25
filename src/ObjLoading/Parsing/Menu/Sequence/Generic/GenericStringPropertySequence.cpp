#include "GenericStringPropertySequence.h"

#include "Parsing/Menu/Matcher/MenuExpressionMatchers.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

#include <utility>

using namespace menu;

GenericStringPropertySequence::GenericStringPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);
    AddLabeledMatchers(MenuExpressionMatchers().Expression(this), MenuExpressionMatchers::LABEL_EXPRESSION);

    AddMatchers({
        create.KeywordIgnoreCase(std::move(keywordName)).Capture(CAPTURE_FIRST_TOKEN),
        create.TextExpression(),
    });
}

void GenericStringPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        const auto value = MenuMatcherFactory::TokenTextExpressionValue(state, result);
        m_set_callback(state, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), value);
    }
}
