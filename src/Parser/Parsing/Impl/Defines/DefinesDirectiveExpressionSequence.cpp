#include "DefinesDirectiveExpressionSequence.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"
#include "DefinesExpressionMatchers.h"

DefinesDirectiveExpressionSequence::DefinesDirectiveExpressionSequence()
{
    const SimpleMatcherFactory create(this);
    AddLabeledMatchers(DefinesExpressionMatchers().Expression(this), SimpleExpressionMatchers::LABEL_EXPRESSION);
    AddMatchers(create.Label(SimpleExpressionMatchers::LABEL_EXPRESSION));
}

void DefinesDirectiveExpressionSequence::ProcessMatch(DefinesDirectiveParsingState* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_expression = DefinesExpressionMatchers(state).ProcessExpression(result);
}
