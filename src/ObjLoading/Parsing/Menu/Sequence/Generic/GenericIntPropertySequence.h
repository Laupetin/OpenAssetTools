#pragma once

#include <string>
#include <functional>

#include "Parsing/Menu/MenuFileParser.h"

namespace menu
{
    class GenericIntPropertySequence final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<void(MenuFileParserState* state, const TokenPos& pos, int value)>;

    private:
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_VALUE = 2;

        const callback_t m_set_callback;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        GenericIntPropertySequence(std::string keywordName, callback_t setCallback);
    };
}
