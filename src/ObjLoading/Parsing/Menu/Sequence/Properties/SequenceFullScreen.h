#pragma once

#include "Parsing/Menu/MenuFileParser.h"

namespace menu
{
    class SequenceFullScreen final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_VALUE = 1;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        SequenceFullScreen();
    };
}
