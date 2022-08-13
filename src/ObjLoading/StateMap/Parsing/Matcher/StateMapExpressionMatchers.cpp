#include "StateMapExpressionMatchers.h"

#include "Parsing/Simple/Expression/SimpleExpressionScopeValue.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

using namespace state_map;

static constexpr int CAPTURE_IDENTIFIER = SimpleExpressionMatchers::CAPTURE_OFFSET_EXPRESSION_EXT + 1;

StateMapExpressionMatchers::StateMapExpressionMatchers()
    : SimpleExpressionMatchers(false, false, false, false, true)
{
}

std::unique_ptr<SimpleExpressionMatchers::matcher_t> StateMapExpressionMatchers::ParseOperandExtension(const supplier_t* labelSupplier) const
{
    const SimpleMatcherFactory create(labelSupplier);

    return create.Or({
        create.Identifier().Capture(CAPTURE_IDENTIFIER)
    });
}

std::unique_ptr<ISimpleExpression> StateMapExpressionMatchers::ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const
{
    const auto& identifierToken = result.NextCapture(CAPTURE_IDENTIFIER);
    auto identifierValue = identifierToken.IdentifierValue();

    if (identifierValue.rfind("mtl", 0) == 0)
        return std::make_unique<SimpleExpressionScopeValue>(identifierValue);

    return std::make_unique<SimpleExpressionValue>(identifierValue);
}
