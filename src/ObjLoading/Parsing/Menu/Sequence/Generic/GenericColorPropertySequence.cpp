#include "GenericColorPropertySequence.h"

#include "Parsing/Menu/Matcher/MenuExpressionMatchers.h"
#include "Parsing/Menu/Matcher/MenuMatcherFactory.h"

#include <utility>

using namespace menu;

GenericColorPropertySequence::GenericColorPropertySequence(std::string keywordName, callback_t setCallback)
    : m_set_callback(std::move(setCallback))
{
    const MenuMatcherFactory create(this);

    AddLabeledMatchers(MenuExpressionMatchers().Expression(this), MenuExpressionMatchers::LABEL_EXPRESSION);
    AddMatchers({
        create.KeywordIgnoreCase(std::move(keywordName)).Capture(CAPTURE_FIRST_TOKEN),
        create.NumericExpression().Tag(TAG_COLOR),                  // r
        create.Optional(create.NumericExpression().Tag(TAG_COLOR)), // g
        create.Optional(create.NumericExpression().Tag(TAG_COLOR)), // b
        create.Optional(create.NumericExpression().Tag(TAG_COLOR)), // a
    });
}

double GenericColorPropertySequence::ReadColorValue(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result)
{
    if (result.PeekAndRemoveIfTag(TAG_COLOR) == TAG_COLOR)
        return MenuMatcherFactory::TokenNumericExpressionValue(state, result);

    return 0.0;
}

void GenericColorPropertySequence::ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    if (m_set_callback)
    {
        CommonColor color{};
        color.r = ReadColorValue(state, result);
        color.g = ReadColorValue(state, result);
        color.b = ReadColorValue(state, result);
        color.a = ReadColorValue(state, result);

        m_set_callback(state, result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), color);
    }
}
