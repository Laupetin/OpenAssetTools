#include "StateMapExpressionMatchers.h"

#include "Parsing/Simple/Expression/SimpleExpressionScopeValue.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

using namespace state_map;

static constexpr int CAPTURE_VALUE = SimpleExpressionMatchers::CAPTURE_OFFSET_EXPRESSION_EXT + 1;

StateMapExpressionMatchers::StateMapExpressionMatchers()
    : StateMapExpressionMatchers(nullptr)
{
}

StateMapExpressionMatchers::StateMapExpressionMatchers(StateMapParserState* state)
    : SimpleExpressionMatchers(false, false, false, false, true),
      m_state(state)
{
}

std::unique_ptr<SimpleExpressionMatchers::matcher_t> StateMapExpressionMatchers::ParseOperandExtension(const supplier_t* labelSupplier) const
{
    const SimpleMatcherFactory create(labelSupplier);

    return create.Or({
        create.Identifier().Capture(CAPTURE_VALUE),
        create.Integer().Capture(CAPTURE_VALUE),
    });
}

std::unique_ptr<ISimpleExpression> StateMapExpressionMatchers::ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const
{
    assert(m_state);

    const auto& valueToken = result.NextCapture(CAPTURE_VALUE);
    auto value = valueToken.m_type == SimpleParserValueType::IDENTIFIER ? valueToken.IdentifierValue() : std::to_string(valueToken.IntegerValue());

    if (value.rfind("mtl", 0) == 0)
    {
        if (m_state->m_valid_vars.find(value) == m_state->m_valid_vars.end())
            throw ParsingException(valueToken.GetPos(), "Unknown variable");

        return std::make_unique<SimpleExpressionScopeValue>(value);
    }

    if (m_state->m_valid_values.find(value) == m_state->m_valid_values.end())
        throw ParsingException(valueToken.GetPos(), "Unknown value");

    return std::make_unique<SimpleExpressionValue>(value);
}
