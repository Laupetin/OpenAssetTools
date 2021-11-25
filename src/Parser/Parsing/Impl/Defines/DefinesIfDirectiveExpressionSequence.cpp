#include "DefinesIfDirectiveExpressionSequence.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"
#include "DefinesIfExpressionMatchers.h"

DefinesIfDirectiveExpressionSequence::DefinesIfDirectiveExpressionSequence()
{
    const SimpleMatcherFactory create(this);
    AddLabeledMatchers(DefinesIfExpressionMatchers().Expression(this), SimpleExpressionMatchers::LABEL_EXPRESSION);
    AddMatchers(create.Label(SimpleExpressionMatchers::LABEL_EXPRESSION));
}

void DefinesIfDirectiveExpressionSequence::ProcessMatch(DefinesIfDirectiveParsingState* state, SequenceResult<SimpleParserValue>& result) const
{
    state->m_expression = DefinesIfExpressionMatchers(state).ProcessExpression(result);
}
