#include "MenuCommonMatchers.h"

#include <list>

#include "MenuMatcherFactory.h"
#include "Parsing/Menu/MenuFileLexing.h"
#include "Parsing/Simple/Expression/SimpleExpressionBinaryOperation.h"
#include "Parsing/Menu/Domain/Expression/CommonExpressionFunctionCall.h"
#include "Parsing/Simple/Expression/SimpleExpressionUnaryOperation.h"

using namespace menu;

static constexpr int TAG_OPERAND = std::numeric_limits<int>::max() - 1;
static constexpr int TAG_EXPRESSION_UNARY_OPERATION = std::numeric_limits<int>::max() - 2;
static constexpr int TAG_EXPRESSION_FUNCTION_CALL = std::numeric_limits<int>::max() - 3;
static constexpr int TAG_EXPRESSION_FUNCTION_CALL_END = std::numeric_limits<int>::max() - 4;
static constexpr int TAG_EXPRESSION_PARENTHESIS = std::numeric_limits<int>::max() - 5;
static constexpr int TAG_EXPRESSION_PARENTHESIS_END = std::numeric_limits<int>::max() - 6;
static constexpr int TAG_EXPRESSION = std::numeric_limits<int>::max() - 7;
static constexpr int TAG_EXPRESSION_BINARY_OPERATION = std::numeric_limits<int>::max() - 8;

static constexpr int CAPTURE_OPERAND = std::numeric_limits<int>::max() - 1;
static constexpr int CAPTURE_UNARY_OPERATION_TYPE = std::numeric_limits<int>::max() - 2;
static constexpr int CAPTURE_BINARY_OPERATION_TYPE = std::numeric_limits<int>::max() - 3;
static constexpr int CAPTURE_FUNCTION_NAME = std::numeric_limits<int>::max() - 4;

std::unique_ptr<ISimpleExpression> MenuCommonMatchers::ProcessExpressionInParenthesis(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result)
{
    auto processedEvaluation = ProcessExpression(state, result);

    if (result.PeekAndRemoveIfTag(TAG_EXPRESSION_PARENTHESIS_END) != TAG_EXPRESSION_PARENTHESIS_END)
        throw ParsingException(TokenPos(), "Expected parenthesis end tag @ ExpressionInParenthesis");

    return processedEvaluation;
}

std::unique_ptr<ISimpleExpression> MenuCommonMatchers::ProcessOperand(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result)
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

std::unique_ptr<ISimpleExpression> MenuCommonMatchers::ProcessFunctionCall(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result)
{
    auto functionCall = std::make_unique<CommonExpressionFunctionCall>(result.NextCapture(CAPTURE_FUNCTION_NAME).IdentifierValue());

    while (result.PeekAndRemoveIfTag(TAG_EXPRESSION_FUNCTION_CALL_END) != TAG_EXPRESSION_FUNCTION_CALL_END)
    {
        functionCall->m_args.emplace_back(ProcessExpression(state, result));
    }

    return std::move(functionCall);
}

std::unique_ptr<ISimpleExpression> MenuCommonMatchers::ProcessExpression(MenuFileParserState* state, SequenceResult<SimpleParserValue>& result)
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

        while (nextTag == TAG_EXPRESSION_UNARY_OPERATION)
        {
            unaryOperations.push_back(result.NextCapture(CAPTURE_UNARY_OPERATION_TYPE).IntegerValue());
            nextTag = result.NextTag();
        }

        switch (nextTag)
        {
        case TAG_EXPRESSION_PARENTHESIS:
            firstStatementPart = ProcessExpressionInParenthesis(state, result);
            break;

        case TAG_EXPRESSION_FUNCTION_CALL:
            firstStatementPart = ProcessFunctionCall(state, result);
            break;

        case TAG_OPERAND:
            firstStatementPart = ProcessOperand(state, result);
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

        if (result.PeekAndRemoveIfTag(TAG_EXPRESSION_BINARY_OPERATION) == TAG_EXPRESSION_BINARY_OPERATION)
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

        return p1.first > p2.first;
    });

    while (!operators.empty())
    {
        const auto [operatorIndex, operatorType] = operators.back();

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

std::unique_ptr<MenuCommonMatchers::matcher_t> MenuCommonMatchers::ParseOperand(const supplier_t* labelSupplier)
{
    const MenuMatcherFactory create(labelSupplier);

    return create.Or({
        create.Integer(),
        create.FloatingPoint(),
        create.String(),
        create.Identifier(),
    }).Tag(TAG_OPERAND).Capture(CAPTURE_OPERAND);
}

std::unique_ptr<MenuCommonMatchers::matcher_t> MenuCommonMatchers::ParseBinaryOperationType(const supplier_t* labelSupplier)
{
    const MenuMatcherFactory create(labelSupplier);

    return create.Or({
        create.Char('+').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::ADD));
        }),
        create.Char('-').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::SUBTRACT));
        }),
        create.Char('*').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::MULTIPLY));
        }),
        create.Char('/').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::DIVIDE));
        }),
        create.Char('%').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::REMAINDER));
        }),
        create.Char('&').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::BITWISE_AND));
        }),
        create.Char('|').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::BITWISE_OR));
        }),
        create.MultiChar(static_cast<int>(MenuFileLexing::MultiChar::SHIFT_LEFT)).Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::SHIFT_LEFT));
        }),
        create.MultiChar(static_cast<int>(MenuFileLexing::MultiChar::SHIFT_RIGHT)).Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::SHIFT_RIGHT));
        }),
        create.Char('>').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::GREATER_THAN));
        }),
        create.MultiChar(static_cast<int>(MenuFileLexing::MultiChar::GREATER_EQUAL)).Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::GREATER_EQUAL_THAN));
        }),
        create.Char('<').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::LESS_THAN));
        }),
        create.MultiChar(static_cast<int>(MenuFileLexing::MultiChar::LESS_EQUAL)).Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::LESS_EQUAL_THAN));
        }),
        create.MultiChar(static_cast<int>(MenuFileLexing::MultiChar::EQUALS)).Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::EQUALS));
        }),
        create.MultiChar(static_cast<int>(MenuFileLexing::MultiChar::NOT_EQUAL)).Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::NOT_EQUAL));
        }),
        create.MultiChar(static_cast<int>(MenuFileLexing::MultiChar::LOGICAL_AND)).Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::AND));
        }),
        create.MultiChar(static_cast<int>(MenuFileLexing::MultiChar::LOGICAL_OR)).Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleBinaryOperationId::OR));
        }),
    }).Capture(CAPTURE_BINARY_OPERATION_TYPE);
}

std::unique_ptr<MenuCommonMatchers::matcher_t> MenuCommonMatchers::ParseFunctionCall(const supplier_t* labelSupplier)
{
    const MenuMatcherFactory create(labelSupplier);

    return create.And({
        create.Identifier().Capture(CAPTURE_FUNCTION_NAME),
        create.Char('('),
        create.Optional(create.And({
            create.Label(LABEL_EXPRESSION),
            create.OptionalLoop(create.And({
                create.Char(','),
                create.Label(LABEL_EXPRESSION)
            })),
        })),
        create.Char(')').Tag(TAG_EXPRESSION_FUNCTION_CALL_END)
    }).Tag(TAG_EXPRESSION_FUNCTION_CALL);
}

std::unique_ptr<MenuCommonMatchers::matcher_t> MenuCommonMatchers::ParseUnaryOperationType(const supplier_t* labelSupplier)
{
    const MenuMatcherFactory create(labelSupplier);

    return create.Or({
        create.Char('!').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleUnaryOperationId::NOT));
        }),
        create.Char('~').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleUnaryOperationId::BITWISE_NOT));
        }),
        create.Char('-').Transform([](const MenuMatcherFactory::token_list_t& values)
        {
            return SimpleParserValue::Integer(values[0].get().GetPos(), static_cast<int>(SimpleUnaryOperationId::NEGATIVE));
        }),
    }).Tag(TAG_EXPRESSION_UNARY_OPERATION).Capture(CAPTURE_UNARY_OPERATION_TYPE);
}

std::unique_ptr<MenuCommonMatchers::matcher_t> MenuCommonMatchers::Expression(const supplier_t* labelSupplier)
{
    const MenuMatcherFactory create(labelSupplier);

    return create.And({
        create.OptionalLoop(ParseUnaryOperationType(labelSupplier)),
        create.Or({
            create.And({
                create.Char('('),
                create.Label(LABEL_EXPRESSION),
                create.Char(')').Tag(TAG_EXPRESSION_PARENTHESIS_END)
            }).Tag(TAG_EXPRESSION_PARENTHESIS),
            ParseFunctionCall(labelSupplier),
            ParseOperand(labelSupplier)
        }),
        create.Optional(create.And({
            ParseBinaryOperationType(labelSupplier),
            create.Label(LABEL_EXPRESSION)
        }).Tag(TAG_EXPRESSION_BINARY_OPERATION))
    }).Tag(TAG_EXPRESSION);
}
