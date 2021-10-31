#pragma once

#include "Parsing/Menu/MenuFileParser.h"

class SequenceCloseBlock final : public MenuFileParser::sequence_t
{
    static constexpr auto CAPTURE_TOKEN = 1;

protected:
    void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

public:
    SequenceCloseBlock();
};
