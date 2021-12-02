#include "GenericColorPropertySequence.h"

#include <utility>

#include "Parsing/Menu/Matcher/MenuExpressionMatchers.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

using namespace menu;

GenericColorPropertySequence::GenericColorPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddLabeledMatchers(MenuExpressionMatchers().Expression(this), MenuExpressionMatchers::LABEL_EXPRESSION);
    AddMatchers({
        create.KeywordIgnoreCase(std::move(keywordName)).Capture(CAPTURE_FIRST_TOKEN),
        create.NumericExpression().Tag(TAG_COLOR), // r
        create.Optional(create.NumericExpression().Tag(TAG_COLOR)), // g
        create.Optional(create.NumericExpression().Tag(TAG_COLOR)), // b
        create.Optional(create.NumericExpression().Tag(TAG_COLOR)) // a
    });
}

double GenericColorPropertySequence::ReadColorValue(SequenceResult<SimpleParserValue>& result)
{
    if (result.PeekAndRemoveIfTag(TAG_COLOR) == TAG_COLOR)
        return MenuMatcherFactory::TokenNumericExpressionValue(result);

    return 0.0;
}

void GenericColorPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        CommonColor color{};
        color.r = ReadColorValue(result);
        color.g = ReadColorValue(result);
        color.b = ReadColorValue(result);
        color.a = ReadColorValue(result);

        m_set_callback(state, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), color);
    }
}
