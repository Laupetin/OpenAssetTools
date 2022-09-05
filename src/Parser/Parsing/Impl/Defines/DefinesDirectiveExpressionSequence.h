#pragma once

#include "DefinesDirectiveParser.h"
#include "DefinesDirectiveParsingState.h"

class DefinesDirectiveExpressionSequence final : public DefinesDirectiveParser::sequence_t
{
public:
    DefinesDirectiveExpressionSequence();

protected:
    void ProcessMatch(DefinesDirectiveParsingState* state, SequenceResult<SimpleParserValue>& result) const override;
};
