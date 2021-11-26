#pragma once

#include <string>
#include <functional>

#include "Parsing/Menu/MenuFileParser.h"

namespace menu
{
    class GenericFloatingPointPropertySequence final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<void(MenuFileParserState* state, const TokenPos& pos, double value)>;

    private:
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

        const callback_t m_set_callback;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        GenericFloatingPointPropertySequence(std::string keywordName, callback_t setCallback);
    };
}
