#pragma once

#include "DefinesIfDirectiveParser.h"
#include "DefinesIfDirectiveParsingState.h"

class DefinesIfDirectiveExpressionSequence final : public DefinesIfDirectiveParser::sequence_t
{
public:
    DefinesIfDirectiveExpressionSequence();

protected:
    void ProcessMatch(DefinesIfDirectiveParsingState* state, SequenceResult<SimpleParserValue>& result) const override;
};
