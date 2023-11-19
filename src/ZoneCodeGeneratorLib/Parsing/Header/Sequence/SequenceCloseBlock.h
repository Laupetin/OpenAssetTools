#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Sequence/AbstractSequence.h"

class SequenceCloseBlock final : public HeaderParser::sequence_t
{
    static constexpr auto TAG_SEMICOLON = 1;

    static constexpr auto CAPTURE_NAME = 1;
    static constexpr auto CAPTURE_CLOSING_PARENTHESIS = 2;

    bool m_semicolon_required;

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;

public:
    explicit SequenceCloseBlock(bool semicolonRequired);
};
