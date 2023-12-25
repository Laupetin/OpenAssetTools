#pragma once

#include "Parsing/Menu/MenuFileParser.h"

#include <functional>
#include <string>

namespace menu
{
    class GenericStringPropertySequence final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<void(MenuFileParserState* state, const TokenPos& pos, const std::string& value)>;

    private:
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

        const callback_t m_set_callback;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        GenericStringPropertySequence(std::string keywordName, callback_t setCallback);
    };
} // namespace menu
