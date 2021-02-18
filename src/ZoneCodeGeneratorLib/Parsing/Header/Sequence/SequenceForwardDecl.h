#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Sequence/AbstractSequence.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"

class SequenceForwardDecl final : public HeaderParser::sequence_t
{
    static constexpr auto TAG_ENUM = 1;
    static constexpr auto TAG_STRUCT = 2;
    static constexpr auto TAG_UNION = 3;

    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_NAME = 2;

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;

public:
    SequenceForwardDecl();
};
