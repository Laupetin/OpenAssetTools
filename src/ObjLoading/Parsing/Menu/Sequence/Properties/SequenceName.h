#pragma once

#include "Parsing/Menu/MenuFileParser.h"

class SequenceName final : public MenuFileParser::sequence_t
{
    static constexpr auto CAPTURE_NAME = 1;

protected:
    void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

public:
    SequenceName();
};
