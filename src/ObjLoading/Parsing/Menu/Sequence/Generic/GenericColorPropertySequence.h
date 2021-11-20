#pragma once

#include <string>
#include <functional>

#include "Parsing/Menu/MenuFileParser.h"
#include "Parsing/Menu/Domain/CommonMenuTypes.h"

namespace menu
{
    class GenericColorPropertySequence final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<void(MenuFileParserState* state, const TokenPos& pos, CommonColor value)>;

    private:
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;
        static constexpr auto CAPTURE_R = 2;
        static constexpr auto CAPTURE_G = 3;
        static constexpr auto CAPTURE_B = 4;
        static constexpr auto CAPTURE_A = 5;

        const callback_t m_set_callback;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    public:
        GenericColorPropertySequence(std::string keywordName, callback_t setCallback);
    };
}
