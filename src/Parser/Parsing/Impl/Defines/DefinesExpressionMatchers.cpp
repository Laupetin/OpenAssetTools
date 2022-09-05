#include "DefinesExpressionMatchers.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

DefinesExpressionMatchers::DefinesExpressionMatchers()
    : DefinesExpressionMatchers(nullptr)
{
}

DefinesExpressionMatchers::DefinesExpressionMatchers(const DefinesDirectiveParsingState* state)
    : SimpleExpressionMatchers(true, false, true, true, false),
      m_state(state)
{
}

std::unique_ptr<SimpleExpressionMatchers::matcher_t> DefinesExpressionMatchers::ParseOperandExtension(const supplier_t* labelSupplier) const
{
    const SimpleMatcherFactory create(labelSupplier);

    return create.And({
        create.Keyword("defined"),
        create.Char('('),
        create.Identifier().Capture(CAPTURE_DEFINE_NAME),
        create.Char(')'),
    });
}

std::unique_ptr<ISimpleExpression> DefinesExpressionMatchers::ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const
{
    const auto& defineCapture = result.NextCapture(CAPTURE_DEFINE_NAME);

    assert(m_state);
    if(m_state && m_state->m_defines.find(defineCapture.IdentifierValue()) != m_state->m_defines.end())
        return std::make_unique<SimpleExpressionValue>(1);

    return std::make_unique<SimpleExpressionValue>(0);
}
