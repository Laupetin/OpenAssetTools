#pragma once

#include "Parsing/Header/Impl/HeaderParser.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"
#include "Parsing/Sequence/AbstractSequence.h"

class SequenceCloseBlock final : public HeaderParser::sequence_t
{
public:
    explicit SequenceCloseBlock(bool semicolonRequired);

private:
    bool m_semicolon_required;

protected:
    void ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const override;
};
