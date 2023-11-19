#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Sequence/AbstractSequence.h"

class SequenceStruct final : public HeaderParser::sequence_t
{
    static constexpr int TAG_TYPEDEF = 1;

    static constexpr int CAPTURE_NAME = 1;
    static constexpr int CAPTURE_ALIGN = 2;
    static constexpr int CAPTURE_PARENT_TYPE = 3;

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;

public:
    SequenceStruct();
};
