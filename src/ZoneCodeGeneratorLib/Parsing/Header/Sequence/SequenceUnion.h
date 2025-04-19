#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Sequence/AbstractSequence.h"

class SequenceUnion final : public HeaderParser::sequence_t
{
public:
    SequenceUnion();

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;
};
