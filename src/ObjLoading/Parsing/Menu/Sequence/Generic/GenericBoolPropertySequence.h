#pragma once

#include <string>
#include <functional>

#include "Parsing/Menu/MenuFileParser.h"

namespace menu
{
    class GenericBoolPropertySequence final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<void(MenuFileParserState* state, bool value)>;

    private:
        static constexpr auto CAPTURE_VALUE = 1;

        const callback_t m_set_callback;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        GenericBoolPropertySequence(std::string keywordName, callback_t setCallback);
    };
}
