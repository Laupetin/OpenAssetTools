#pragma once

#include "Parsing/Menu/MenuFileParser.h"

namespace menu
{
    class SequenceLoadMenu final : public MenuFileParser::sequence_t
    {
        static constexpr auto CAPTURE_MENU_NAME = 1;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        SequenceLoadMenu();
    };
}
