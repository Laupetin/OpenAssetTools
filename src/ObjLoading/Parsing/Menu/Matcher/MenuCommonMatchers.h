#pragma once

#include <limits>
#include <memory>

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Matcher/MatcherLabel.h"
#include "Parsing/Menu/MenuFileParser.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"
#include "Parsing/Sequence/SequenceResult.h"
#include "Parsing/Simple/SimpleParserValue.h"

namespace menu
{
    class MenuCommonMatchers
    {
    public:
        using matcher_t = MenuFileParser::sequence_t::matcher_t;
        typedef IMatcherForLabelSupplier<SimpleParserValue> supplier_t;

        static constexpr int LABEL_EXPRESSION = std::numeric_limits<int>::max() - 1;

    private:
        static std::unique_ptr<matcher_t> ParseBinaryOperationType(const supplier_t* labelSupplier);
        static std::unique_ptr<matcher_t> ParseOperand(const supplier_t* labelSupplier);
        static std::unique_ptr<matcher_t> ParseFunctionCall(const supplier_t* labelSupplier);
        static std::unique_ptr<matcher_t> ParseUnaryOperationType(const supplier_t* labelSupplier);

        static std::unique_ptr<ISimpleExpression> ProcessExpressionInParenthesis(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);
        static std::unique_ptr<ISimpleExpression> ProcessOperand(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);
        static std::unique_ptr<ISimpleExpression> ProcessFunctionCall(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);

    public:
        static std::unique_ptr<matcher_t> Expression(const supplier_t* labelSupplier);
        static std::unique_ptr<ISimpleExpression> ProcessExpression(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result);
    };
}
