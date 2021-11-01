#pragma once

#include "Parsing/Menu/MenuFileParser.h"

namespace menu
{
    class SequenceFunctionDef final : public MenuFileParser::sequence_t
    {
    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        SequenceFunctionDef();
    };
}
