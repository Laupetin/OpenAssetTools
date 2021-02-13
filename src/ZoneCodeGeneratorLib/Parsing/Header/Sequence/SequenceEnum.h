#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Sequence/AbstractSequence.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"

class SequenceEnum final : public HeaderParser::sequence_t
{
    static constexpr auto TAG_TYPEDEF = 1;

    static constexpr auto CAPTURE_NAME = 1;
    static constexpr auto CAPTURE_PARENT_TYPE = 2;

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;

public:
    SequenceEnum();
};
