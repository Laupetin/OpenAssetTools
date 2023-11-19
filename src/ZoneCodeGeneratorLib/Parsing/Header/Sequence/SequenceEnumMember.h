#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Sequence/AbstractSequence.h"

class SequenceEnumMember final : public HeaderParser::sequence_t
{
    static constexpr auto CAPTURE_NAME = 1;
    static constexpr auto CAPTURE_VALUE = 2;

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;

public:
    SequenceEnumMember();
};
