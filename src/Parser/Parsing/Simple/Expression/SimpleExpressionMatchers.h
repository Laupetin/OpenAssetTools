#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Matcher/MatcherLabel.h"
#include "Parsing/Sequence/SequenceResult.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "SimpleExpressionBinaryOperation.h"
#include "SimpleExpressionUnaryOperation.h"
#include "Utils/ClassUtils.h"

#include <Parsing/Simple/SimpleLexer.h>
#include <memory>

class SimpleExpressionMatchers
{
public:
    using matcher_t = AbstractMatcher<SimpleParserValue>;
    typedef IMatcherForLabelSupplier<SimpleParserValue> supplier_t;

    static constexpr auto CAPTURE_OFFSET_EXPRESSION = 7000;
    static constexpr auto TAG_OFFSET_EXPRESSION = 7000;
    static constexpr auto CAPTURE_OFFSET_EXPRESSION_EXT = 8000;
    static constexpr auto TAG_OFFSET_EXPRESSION_EXT = 8000;
    static constexpr auto LABEL_EXPRESSION = 7000;
    static constexpr auto MULTI_TOKEN_OFFSET_UNARY = 700;
    static constexpr auto MULTI_TOKEN_OFFSET_BINARY = 720;

private:
    bool m_enable_string_operands;
    bool m_enable_identifier_operands;
    bool m_enable_floating_point_operands;
    bool m_enable_int_operands;
    bool m_enable_conditional_operator;

public:
    SimpleExpressionMatchers();
    SimpleExpressionMatchers(
        bool enableStringOperands, bool enableIdentifierOperands, bool enableFloatingPointOperands, bool enableIntOperands, bool enableConditionalOperator);
    virtual ~SimpleExpressionMatchers();
    SimpleExpressionMatchers(const SimpleExpressionMatchers& other) = default;
    SimpleExpressionMatchers(SimpleExpressionMatchers&& other) noexcept = default;
    SimpleExpressionMatchers& operator=(const SimpleExpressionMatchers& other) = default;
    SimpleExpressionMatchers& operator=(SimpleExpressionMatchers&& other) noexcept = default;

protected:
    virtual std::unique_ptr<matcher_t> ParseOperandExtension(const supplier_t* labelSupplier) const;
    virtual std::unique_ptr<ISimpleExpression> ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const;

    _NODISCARD virtual std::vector<const SimpleExpressionUnaryOperationType*> EnabledUnaryOperations() const;
    _NODISCARD virtual std::vector<const SimpleExpressionBinaryOperationType*> EnabledBinaryOperations() const;

private:
    std::unique_ptr<matcher_t> ParseBinaryOperationType(const supplier_t* labelSupplier) const;
    std::unique_ptr<matcher_t> ParseOperand(const supplier_t* labelSupplier) const;
    std::unique_ptr<matcher_t> ParseUnaryOperationType(const supplier_t* labelSupplier) const;
    std::unique_ptr<matcher_t> ParseConditionalOperator(const supplier_t* labelSupplier) const;

    std::unique_ptr<ISimpleExpression> ProcessExpressionInParenthesis(SequenceResult<SimpleParserValue>& result) const;
    std::unique_ptr<ISimpleExpression> ProcessConditionalOperation(std::unique_ptr<ISimpleExpression> condition,
                                                                   SequenceResult<SimpleParserValue>& result) const;
    std::unique_ptr<ISimpleExpression> ProcessOperand(SequenceResult<SimpleParserValue>& result) const;

public:
    std::unique_ptr<matcher_t> Expression(const supplier_t* labelSupplier) const;
    std::unique_ptr<ISimpleExpression> ProcessExpression(SequenceResult<SimpleParserValue>& result) const;

    virtual void ApplyTokensToLexerConfig(SimpleLexer::Config& lexerConfig);
};
