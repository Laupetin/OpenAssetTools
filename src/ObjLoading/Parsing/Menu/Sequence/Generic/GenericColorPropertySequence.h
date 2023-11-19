#pragma once

#include "Parsing/Menu/Domain/CommonMenuTypes.h"
#include "Parsing/Menu/MenuFileParser.h"

#include <functional>
#include <string>

namespace menu
{
    class GenericColorPropertySequence final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<void(MenuFileParserState* state, const TokenPos& pos, CommonColor value)>;

    private:
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

        static constexpr auto TAG_COLOR = 1;

        const callback_t m_set_callback;

        static double ReadColorValue(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        GenericColorPropertySequence(std::string keywordName, callback_t setCallback);
    };
} // namespace menu
