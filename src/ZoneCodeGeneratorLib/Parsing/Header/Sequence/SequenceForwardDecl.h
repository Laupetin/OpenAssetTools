#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Sequence/AbstractSequence.h"

class SequenceForwardDecl final : public HeaderParser::sequence_t
{
public:
    SequenceForwardDecl();

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;
};
