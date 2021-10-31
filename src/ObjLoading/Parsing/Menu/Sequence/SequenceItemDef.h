#pragma once

#include "Parsing/Menu/MenuFileParser.h"

class SequenceItemDef final : public MenuFileParser::sequence_t
{
protected:
    void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

public:
    SequenceItemDef();
};
