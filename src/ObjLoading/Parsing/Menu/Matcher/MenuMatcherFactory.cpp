#include "MenuMatcherFactory.h"

#include "MenuExpressionMatchers.h"

#include <sstream>

using namespace menu;

MenuMatcherFactory::MenuMatcherFactory(const IMatcherForLabelSupplier<SimpleParserValue>* labelSupplier)
    : SimpleMatcherFactory(labelSupplier)
{
}

MatcherFactoryWrapper<SimpleParserValue> MenuMatcherFactory::StringChain() const
{
    return Or({
        And({
                String(),
                Loop(String()),
            })
            .Transform(
                [](const token_list_t& tokens) -> SimpleParserValue
                {
                    std::ostringstream ss;

                    for (const auto& token : tokens)
                    {
                        ss << token.get().StringValue();
                    }

                    return SimpleParserValue::String(tokens[0].get().GetPos(), new std::string(ss.str()));
                }),
        String(),
    });
}

MatcherFactoryWrapper<SimpleParserValue> MenuMatcherFactory::Text() const
{
    return MatcherFactoryWrapper(Or({
        StringChain(),
        Identifier(),
    }));
}

MatcherFactoryWrapper<SimpleParserValue> MenuMatcherFactory::TextNoChain() const
{
    return MatcherFactoryWrapper(Or({
        String(),
        Identifier(),
    }));
}

MatcherFactoryWrapper<SimpleParserValue> MenuMatcherFactory::Numeric() const
{
    return MatcherFactoryWrapper(Or({
        FloatingPoint(),
        Integer(),
    }));
}

MatcherFactoryWrapper<SimpleParserValue> MenuMatcherFactory::IntExpression() const
{
    return MatcherFactoryWrapper(Or({
        Integer().Tag(TAG_INT).Capture(CAPTURE_INT),
        And({
                Char('(').Capture(CAPTURE_FIRST_TOKEN),
                Label(MenuExpressionMatchers::LABEL_EXPRESSION),
                Char(')'),
            })
            .Tag(TAG_EXPRESSION),
    }));
}

MatcherFactoryWrapper<SimpleParserValue> MenuMatcherFactory::NumericExpression() const
{
    return MatcherFactoryWrapper(Or({
        Numeric().Tag(TAG_NUMERIC).Capture(CAPTURE_NUMERIC),
        And({
                Char('(').Capture(CAPTURE_FIRST_TOKEN),
                Label(MenuExpressionMatchers::LABEL_EXPRESSION),
                Char(')'),
            })
            .Tag(TAG_EXPRESSION),
    }));
}

int MenuMatcherFactory::TokenNumericIntValue(const SimpleParserValue& value)
{
    if (value.m_type == SimpleParserValueType::FLOATING_POINT)
    {
        return static_cast<int>(value.FloatingPointValue());
    }

    return value.IntegerValue();
}

double MenuMatcherFactory::TokenNumericFloatingPointValue(const SimpleParserValue& value)
{
    if (value.m_type == SimpleParserValueType::INTEGER)
    {
        return value.IntegerValue();
    }

    return value.FloatingPointValue();
}

std::string& MenuMatcherFactory::TokenTextValue(const SimpleParserValue& value)
{
    if (value.m_type == SimpleParserValueType::IDENTIFIER)
    {
        return value.IdentifierValue();
    }

    return value.StringValue();
}

int MenuMatcherFactory::TokenIntExpressionValue(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result)
{
    const auto nextTag = result.PeekTag();

    assert(nextTag == TAG_INT || nextTag == TAG_EXPRESSION);
    if (nextTag == TAG_INT)
    {
        result.NextTag();
        return result.NextCapture(CAPTURE_INT).IntegerValue();
    }

    if (nextTag == TAG_EXPRESSION)
    {
        result.NextTag();
        const auto expression = MenuExpressionMatchers(state).ProcessExpression(result);

        if (!expression || !expression->IsStatic())
            throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), "Not a valid static expression");

        const auto value = expression->EvaluateStatic();

        if (value.m_type != SimpleExpressionValue::Type::INT)
            throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), "Expression MUST be int type");

        return value.m_int_value;
    }

    throw ParsingException(TokenPos(), "TokenIntExpressionValue must be expression or int");
}

double MenuMatcherFactory::TokenNumericExpressionValue(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result)
{
    const auto nextTag = result.PeekTag();

    assert(nextTag == TAG_NUMERIC || nextTag == TAG_EXPRESSION);
    if (nextTag == TAG_NUMERIC)
    {
        result.NextTag();
        return TokenNumericFloatingPointValue(result.NextCapture(CAPTURE_NUMERIC));
    }

    if (nextTag == TAG_EXPRESSION)
    {
        result.NextTag();
        const auto expression = MenuExpressionMatchers(state).ProcessExpression(result);

        if (!expression || !expression->IsStatic())
            throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), "Not a valid static expression");

        const auto value = expression->EvaluateStatic();

        if (value.m_type == SimpleExpressionValue::Type::INT)
            return value.m_int_value;
        if (value.m_type == SimpleExpressionValue::Type::DOUBLE)
            return value.m_double_value;

        throw ParsingException(result.NextCapture(CAPTURE_FIRST_TOKEN).GetPos(), "Expression MUST be numeric type");
    }

    throw ParsingException(TokenPos(), "TokenNumericExpressionValue must be expression or numeric");
}
