#pragma once

#include <string>
#include <functional>

#include "Parsing/Menu/MenuFileParser.h"
#include "Parsing/Menu/Domain/Expression/ICommonExpression.h"

namespace menu
{
    class GenericExpressionPropertySequence final : public MenuFileParser::sequence_t
    {
    public:
        using callback_t = std::function<void(MenuFileParserState* state, const TokenPos& pos, std::unique_ptr<ICommonExpression> value)>;

    private:
        static constexpr auto CAPTURE_FIRST_TOKEN = 1;

        const callback_t m_set_callback;

    protected:
        void ProcessMatch(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

    private:
        explicit GenericExpressionPropertySequence(callback_t setCallback);

    public:
        static std::unique_ptr<GenericExpressionPropertySequence> WithKeyword(std::string keyword, callback_t setCallback);
        static std::unique_ptr<GenericExpressionPropertySequence> WithKeywords(std::initializer_list<std::string> keywords, callback_t setCallback);
        static std::unique_ptr<GenericExpressionPropertySequence> WithKeywordAndBool(std::string keyword, callback_t setCallback);
    };
}
