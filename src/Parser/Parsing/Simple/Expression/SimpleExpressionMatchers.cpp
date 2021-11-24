#include "SimpleExpressionMatchers.h"

#include <list>

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"
#include "Parsing/Simple/Expression/SimpleExpressionBinaryOperation.h"
#include "Parsing/Simple/Expression/SimpleExpressionUnaryOperation.h"

static constexpr int TAG_EXPRESSION = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION + 1;
static constexpr int TAG_OPERAND = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION + 2;
static constexpr int TAG_UNARY_OPERATION = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION + 3;
static constexpr int TAG_PARENTHESIS = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION + 4;
static constexpr int TAG_PARENTHESIS_END = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION + 5;
static constexpr int TAG_BINARY_OPERATION = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION + 6;
static constexpr int TAG_OPERAND_EXT = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION + 7;
static constexpr int TAG_OPERAND_EXT_END = SimpleExpressionMatchers::TAG_OFFSET_EXPRESSION + 8;

static constexpr int CAPTURE_OPERAND = SimpleExpressionMatchers::CAPTURE_OFFSET_EXPRESSION + 1;
static constexpr int CAPTURE_UNARY_OPERATION_TYPE = SimpleExpressionMatchers::CAPTURE_OFFSET_EXPRESSION + 2;
static constexpr int CAPTURE_BINARY_OPERATION_TYPE = SimpleExpressionMatchers::CAPTURE_OFFSET_EXPRESSION + 3;

SimpleExpressionMatchers::SimpleExpressionMatchers()
    : SimpleExpressionMatchers(true, true, true, true)
{
}

SimpleExpressionMatchers::SimpleExpressionMatchers(const bool enableStringOperands, const bool enableIdentifierOperands, const bool enableFloatingPointOperands, const bool enableIntOperands)
    : m_enable_string_operands(enableStringOperands),
      m_enable_identifier_operands(enableIdentifierOperands),
      m_enable_floating_point_operands(enableFloatingPointOperands),
      m_enable_int_operands(enableIntOperands)
{
}

SimpleExpressionMatchers::~SimpleExpressionMatchers()
= default;

void SimpleExpressionMatchers::ApplyTokensToLexerConfig(SimpleLexer::Config& lexerConfig)
{
    for (const auto* unaryOperation : SimpleExpressionUnaryOperationType::ALL_OPERATION_TYPES)
    {
        if (unaryOperation->m_syntax.size() <= 1)
            continue;

        lexerConfig.m_multi_character_tokens.emplace_back(MULTI_TOKEN_OFFSET_UNARY + static_cast<int>(unaryOperation->m_id), unaryOperation->m_syntax);
    }

    for (const auto* binaryOperation : SimpleExpressionBinaryOperationType::ALL_OPERATION_TYPES)
    {
        if (binaryOperation->m_syntax.size() <= 1)
            continue;

        lexerConfig.m_multi_character_tokens.emplace_back(MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(binaryOperation->m_id), binaryOperation->m_syntax);
    }
}

std::vector<const SimpleExpressionUnaryOperationType*> SimpleExpressionMatchers::EnabledUnaryOperations() const
{
    return std::vector(&SimpleExpressionUnaryOperationType::ALL_OPERATION_TYPES[0],
                       &SimpleExpressionUnaryOperationType::ALL_OPERATION_TYPES[std::extent_v<decltype(SimpleExpressionUnaryOperationType::ALL_OPERATION_TYPES)>]);
}

std::vector<const SimpleExpressionBinaryOperationType*> SimpleExpressionMatchers::EnabledBinaryOperations() const
{
    return std::vector(&SimpleExpressionBinaryOperationType::ALL_OPERATION_TYPES[0],
                       &SimpleExpressionBinaryOperationType::ALL_OPERATION_TYPES[std::extent_v<decltype(SimpleExpressionBinaryOperationType::ALL_OPERATION_TYPES)>]);
}

std::unique_ptr<ISimpleExpression> SimpleExpressionMatchers::ProcessExpressionInParenthesis(SequenceResult<SimpleParserValue>& result) const
{
    auto processedEvaluation = ProcessExpression(result);

    if (result.PeekAndRemoveIfTag(TAG_PARENTHESIS_END) != TAG_PARENTHESIS_END)
        throw ParsingException(TokenPos(), "Expected parenthesis end tag @ ExpressionInParenthesis");

    return processedEvaluation;
}

std::unique_ptr<ISimpleExpression> SimpleExpressionMatchers::ProcessOperand(SequenceResult<SimpleParserValue>& result) const
{
    const auto& operandToken = result.NextCapture(CAPTURE_OPERAND);

    switch (operandToken.m_type)
    {
    case SimpleParserValueType::INTEGER:
        return std::make_unique<SimpleExpressionValue>(operandToken.IntegerValue());
    case SimpleParserValueType::FLOATING_POINT:
        return std::make_unique<SimpleExpressionValue>(operandToken.FloatingPointValue());
    case SimpleParserValueType::STRING:
        return std::make_unique<SimpleExpressionValue>(operandToken.StringValue());
    case SimpleParserValueType::IDENTIFIER:
        return std::make_unique<SimpleExpressionValue>(operandToken.IdentifierValue());
    default:
        throw ParsingException(TokenPos(), "Unknown operand type @ Operand");
    }
}

std::unique_ptr<ISimpleExpression> SimpleExpressionMatchers::ProcessOperandExtension(SequenceResult<SimpleParserValue>& result) const
{
    return nullptr;
}

std::unique_ptr<ISimpleExpression> SimpleExpressionMatchers::ProcessExpression(SequenceResult<SimpleParserValue>& result) const
{
    if (result.PeekAndRemoveIfTag(TAG_EXPRESSION) != TAG_EXPRESSION)
        return nullptr;

    std::vector<std::unique_ptr<ISimpleExpression>> operands;
    std::list<std::pair<unsigned, const SimpleExpressionBinaryOperationType*>> operators;

    while (true)
    {
        std::unique_ptr<ISimpleExpression> firstStatementPart;
        std::vector<int> unaryOperations;
        auto nextTag = result.NextTag();

        while (nextTag == TAG_UNARY_OPERATION)
        {
            unaryOperations.push_back(result.NextCapture(CAPTURE_UNARY_OPERATION_TYPE).IntegerValue());
            nextTag = result.NextTag();
        }

        switch (nextTag)
        {
        case TAG_PARENTHESIS:
            firstStatementPart = ProcessExpressionInParenthesis(result);
            break;

        case TAG_OPERAND:
            firstStatementPart = ProcessOperand(result);
            break;

        case TAG_OPERAND_EXT:
            firstStatementPart = ProcessOperandExtension(result);
            if (result.PeekAndRemoveIfTag(TAG_OPERAND_EXT_END) != TAG_OPERAND_EXT_END)
                throw ParsingException(TokenPos(), "Unclosed operand extension @ Expression");
            break;

        default:
            throw ParsingException(TokenPos(), "Invalid followup tag @ Expression");
        }

        for (auto i = unaryOperations.size(); i > 0; i--)
        {
            const auto operationIndex = unaryOperations[i - 1];
            if (operationIndex < 0 || operationIndex >= static_cast<int>(SimpleUnaryOperationId::COUNT))
                throw ParsingException(TokenPos(), "Invalid unary operation id @ Expression");
            firstStatementPart = std::make_unique<SimpleExpressionUnaryOperation>(SimpleExpressionUnaryOperationType::ALL_OPERATION_TYPES[operationIndex],
                                                                                  std::move(firstStatementPart));
        }

        operands.emplace_back(std::move(firstStatementPart));

        if (result.PeekAndRemoveIfTag(TAG_BINARY_OPERATION) == TAG_BINARY_OPERATION)
        {
            const auto operationIndex = result.NextCapture(CAPTURE_BINARY_OPERATION_TYPE).IntegerValue();
            if (operationIndex < 0 || operationIndex >= static_cast<int>(SimpleBinaryOperationId::COUNT))
                throw ParsingException(TokenPos(), "Invalid binary operation id @ Expression");

            operators.emplace_back(operators.size(), SimpleExpressionBinaryOperationType::ALL_OPERATION_TYPES[operationIndex]);
        }
        else
            break;

        if (result.PeekAndRemoveIfTag(TAG_EXPRESSION) != TAG_EXPRESSION)
            throw ParsingException(TokenPos(), "Expected EvaluationTag @ Evaluation");
    }

    operators.sort([](const std::pair<unsigned, const SimpleExpressionBinaryOperationType*>& p1, const std::pair<unsigned, const SimpleExpressionBinaryOperationType*>& p2)
    {
        if (p1.second->m_precedence != p2.second->m_precedence)
            return p1.second->m_precedence > p2.second->m_precedence;

        return p1.first < p2.first;
    });

    while (!operators.empty())
    {
        const auto& [operatorIndex, operatorType] = operators.back();

        auto operation = std::make_unique<SimpleExpressionBinaryOperation>(operatorType, std::move(operands[operatorIndex]), std::move(operands[operatorIndex + 1]));
        operands.erase(operands.begin() + static_cast<int>(operatorIndex));
        operands[operatorIndex] = std::move(operation);

        operators.pop_back();

        for (auto& [opIndex, _] : operators)
        {
            if (opIndex > operatorIndex)
                opIndex--;
        }
    }

    return std::move(operands.front());
}

std::unique_ptr<SimpleExpressionMatchers::matcher_t> SimpleExpressionMatchers::ParseOperand(const supplier_t* labelSupplier) const
{
    const SimpleMatcherFactory create(labelSupplier);

    std::vector<std::unique_ptr<matcher_t>> operandMatchers;
    operandMatchers.reserve(4);

    if (m_enable_string_operands)
        operandMatchers.emplace_back(create.String());

    if (m_enable_identifier_operands)
        operandMatchers.emplace_back(create.Identifier());

    if (m_enable_floating_point_operands)
        operandMatchers.emplace_back(create.FloatingPoint());

    if (m_enable_int_operands)
        operandMatchers.emplace_back(create.Integer());

    return create.Or(std::move(operandMatchers)).Tag(TAG_OPERAND).Capture(CAPTURE_OPERAND);
}

std::unique_ptr<SimpleExpressionMatchers::matcher_t> SimpleExpressionMatchers::ParseBinaryOperationType(const supplier_t* labelSupplier) const
{
    const SimpleMatcherFactory create(labelSupplier);

    const auto enabledBinaryOperations = EnabledBinaryOperations();
    std::vector<std::unique_ptr<matcher_t>> binaryOperationsMatchers;
    binaryOperationsMatchers.reserve(enabledBinaryOperations.size());

    for (const auto* enabledBinaryOperation : enabledBinaryOperations)
    {
        if (enabledBinaryOperation->m_syntax.size() > 1)
        {
            binaryOperationsMatchers.emplace_back(
                create.MultiChar(MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(enabledBinaryOperation->m_id))
                      .Transform([enabledBinaryOperation](const SimpleMatcherFactory::token_list_t& values)
                      {
                          return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(enabledBinaryOperation->m_id));
                      }));
        }
        else if (!enabledBinaryOperation->m_syntax.empty())
        {
            binaryOperationsMatchers.emplace_back(
                create.Char(enabledBinaryOperation->m_syntax[0])
                      .Transform([enabledBinaryOperation](const SimpleMatcherFactory::token_list_t& values)
                      {
                          return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(enabledBinaryOperation->m_id));
                      }));
        }
    }

    return create.Or(std::move(binaryOperationsMatchers)).Capture(CAPTURE_BINARY_OPERATION_TYPE);
}

std::unique_ptr<SimpleExpressionMatchers::matcher_t> SimpleExpressionMatchers::ParseOperandExtension(const supplier_t* labelSupplier) const
{
    const SimpleMatcherFactory create(labelSupplier);

    return create.False();
}

std::unique_ptr<SimpleExpressionMatchers::matcher_t> SimpleExpressionMatchers::ParseUnaryOperationType(const supplier_t* labelSupplier) const
{
    const SimpleMatcherFactory create(labelSupplier);

    const auto enabledUnaryOperations = EnabledUnaryOperations();
    std::vector<std::unique_ptr<matcher_t>> unaryOperationsMatchers;
    unaryOperationsMatchers.reserve(enabledUnaryOperations.size());

    for (const auto* enabledUnaryOperation : enabledUnaryOperations)
    {
        if (enabledUnaryOperation->m_syntax.size() > 1)
        {
            unaryOperationsMatchers.emplace_back(
                create.MultiChar(MULTI_TOKEN_OFFSET_UNARY + static_cast<int>(enabledUnaryOperation->m_id))
                .Transform([enabledUnaryOperation](const SimpleMatcherFactory::token_list_t& values)
                    {
                        return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(enabledUnaryOperation->m_id));
                    }));
        }
        else if (!enabledUnaryOperation->m_syntax.empty())
        {
            unaryOperationsMatchers.emplace_back(
                create.Char(enabledUnaryOperation->m_syntax[0])
                .Transform([enabledUnaryOperation](const SimpleMatcherFactory::token_list_t& values)
                    {
                        return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(enabledUnaryOperation->m_id));
                    }));
        }
    }

    return create.Or(std::move(unaryOperationsMatchers)).Tag(TAG_UNARY_OPERATION).Capture(CAPTURE_UNARY_OPERATION_TYPE);
}

std::unique_ptr<SimpleExpressionMatchers::matcher_t> SimpleExpressionMatchers::Expression(const supplier_t* labelSupplier) const
{
    const SimpleMatcherFactory create(labelSupplier);

    return create.And({
        create.OptionalLoop(ParseUnaryOperationType(labelSupplier)),
        create.Or({
            create.And({
                create.Char('('),
                create.Label(LABEL_EXPRESSION),
                create.Char(')').Tag(TAG_PARENTHESIS_END)
            }).Tag(TAG_PARENTHESIS),
            create.And({
                create.True().Tag(TAG_OPERAND_EXT),
                ParseOperandExtension(labelSupplier),
                create.True().Tag(TAG_OPERAND_EXT_END)
            }),
            ParseOperand(labelSupplier)
        }),
        create.Optional(create.And({
            ParseBinaryOperationType(labelSupplier),
            create.Label(LABEL_EXPRESSION)
        }).Tag(TAG_BINARY_OPERATION))
    }).Tag(TAG_EXPRESSION);
}
