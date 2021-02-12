#pragma once

#include "Parsing/Sequence/AbstractSequence.h"
#include "Parsing/Header/Impl/HeaderParserState.h"
#include "Parsing/Header/Impl/HeaderParserValue.h"

class SequenceNamespace final : public AbstractSequence<HeaderParserValue, HeaderParserState>
{
    static constexpr int CAPTURE_NAME = 0;

protected:
    void ProcessMatch(HeaderParserState* state, const SequenceResult<HeaderParserValue>& result) const override;

public:
    SequenceNamespace();
};
