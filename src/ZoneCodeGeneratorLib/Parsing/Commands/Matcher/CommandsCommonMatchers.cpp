#include "CommandsCommonMatchers.h"

#include "CommandsMatcherFactory.h"
#include "Domain/Evaluation/OperandDynamic.h"
#include "Domain/Evaluation/OperandStatic.h"
#include "Domain/Evaluation/Operation.h"

#include <list>
#include <sstream>
#include <type_traits>
#include <vector>

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::Typename(const supplier_t* labelSupplier)
{
    static constexpr const char* BUILT_IN_TYPE_NAMES[]{
        "unsigned",
        "char",
        "short",
        "int",
        "long",
    };
    static_assert(std::extent<decltype(BUILT_IN_TYPE_NAMES)>::value
                  == static_cast<int>(CommandsParserValueType::BUILT_IN_LAST) - static_cast<int>(CommandsParserValueType::BUILT_IN_FIRST) + 1);

    const CommandsMatcherFactory create(labelSupplier);

    return create.Or({
        create
            .And({
                create.Optional(create.Type(CommandsParserValueType::UNSIGNED)),
                create.Or({
                    create.Type(CommandsParserValueType::CHAR),
                    create.Type(CommandsParserValueType::SHORT),
                    create.Type(CommandsParserValueType::INT),
                    create.And({
                        create.Type(CommandsParserValueType::LONG),
                        create.Optional(create.Type(CommandsParserValueType::LONG)),
                    }),
                }),
            })
            .Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    std::ostringstream str;
                    auto first = true;

                    for (const auto& token : values)
                    {
                        if (first)
                            first = false;
                        else
                            str << " ";
                        str << BUILT_IN_TYPE_NAMES[static_cast<int>(token.get().m_type) - static_cast<int>(CommandsParserValueType::BUILT_IN_FIRST)];
                    }

                    return CommandsParserValue::TypeName(values[0].get().GetPos(), new std::string(str.str()));
                }),
        create
            .And({
                create.Identifier(),
                create.OptionalLoop(create.And({
                    create.Char(':'),
                    create.Char(':'),
                    create.Identifier(),
                })),
            })
            .Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    std::ostringstream str;
                    str << values[0].get().IdentifierValue();

                    for (auto i = 3u; i < values.size(); i += 3)
                        str << "::" << values[i].get().IdentifierValue();

                    return CommandsParserValue::TypeName(values[0].get().GetPos(), new std::string(str.str()));
                }),
    });
}

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::ArrayDef(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create
        .And({
            create.Char('['),
            create.Or({
                create.Integer(),
                create.Identifier(),
            }),
            create.Char(']'),
        })
        .Transform(
            [](CommandsMatcherFactory::token_list_t& values)
            {
                if (values[1].get().m_type == CommandsParserValueType::INTEGER)
                    return CommandsParserValue::Integer(values[1].get().GetPos(), values[1].get().IntegerValue());

                return CommandsParserValue::Identifier(values[1].get().GetPos(), new std::string(values[1].get().IdentifierValue()));
            });
}

static constexpr int TAG_OPERAND = std::numeric_limits<int>::max() - 1;
static constexpr int TAG_OPERAND_TYPENAME = std::numeric_limits<int>::max() - 2;
static constexpr int TAG_OPERAND_ARRAY = std::numeric_limits<int>::max() - 3;
static constexpr int TAG_OPERAND_ARRAY_END = std::numeric_limits<int>::max() - 4;
static constexpr int TAG_OPERAND_INTEGER = std::numeric_limits<int>::max() - 5;
static constexpr int TAG_EVALUATION_NOT = std::numeric_limits<int>::max() - 6;
static constexpr int TAG_EVALUATION_PARENTHESIS = std::numeric_limits<int>::max() - 7;
static constexpr int TAG_EVALUATION_PARENTHESIS_END = std::numeric_limits<int>::max() - 8;
static constexpr int TAG_EVALUATION = std::numeric_limits<int>::max() - 9;
static constexpr int TAG_EVALUATION_OPERATION = std::numeric_limits<int>::max() - 10;

static constexpr int CAPTURE_OPERAND_TYPENAME = std::numeric_limits<int>::max() - 1;
static constexpr int CAPTURE_OPERAND_ARRAY = std::numeric_limits<int>::max() - 2;
static constexpr int CAPTURE_OPERAND_INTEGER = std::numeric_limits<int>::max() - 3;
static constexpr int CAPTURE_BINARY_OPERATION_TYPE = std::numeric_limits<int>::max() - 4;

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::ParseOperandArray(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create
        .And({
            create.Char('['),
            create.Label(LABEL_EVALUATION),
            create.Char(']').Tag(TAG_OPERAND_ARRAY_END),
        })
        .Tag(TAG_OPERAND_ARRAY);
}

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::ParseOperand(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create
        .Or({
            create
                .And({
                    create.Label(LABEL_TYPENAME).Capture(CAPTURE_OPERAND_TYPENAME),
                    create.OptionalLoop(MatcherFactoryWrapper<CommandsParserValue>(ParseOperandArray(labelSupplier)).Capture(CAPTURE_OPERAND_ARRAY)),
                })
                .Tag(TAG_OPERAND_TYPENAME),
            create.Integer().Tag(TAG_OPERAND_INTEGER).Capture(CAPTURE_OPERAND_INTEGER),
        })
        .Tag(TAG_OPERAND);
}

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::ParseOperationType(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create
        .Or({
            create.Char('+').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_ADD);
                }),
            create.Char('-').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_SUBTRACT);
                }),
            create.Char('*').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_MULTIPLY);
                }),
            create.Char('/').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_DIVIDE);
                }),
            create.Char('%').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_REMAINDER);
                }),
            create.Char('&').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_BITWISE_AND);
                }),
            create.Char('^').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_BITWISE_XOR);
                }),
            create.Char('|').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_BITWISE_OR);
                }),
            create.Type(CommandsParserValueType::SHIFT_LEFT)
                .Transform(
                    [](CommandsMatcherFactory::token_list_t& values)
                    {
                        return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_SHIFT_LEFT);
                    }),
            create.Type(CommandsParserValueType::SHIFT_RIGHT)
                .Transform(
                    [](CommandsMatcherFactory::token_list_t& values)
                    {
                        return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_SHIFT_RIGHT);
                    }),
            create.Char('>').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_GREATER_THAN);
                }),
            create.Type(CommandsParserValueType::GREATER_EQUAL)
                .Transform(
                    [](CommandsMatcherFactory::token_list_t& values)
                    {
                        return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_GREATER_EQUAL_THAN);
                    }),
            create.Char('<').Transform(
                [](CommandsMatcherFactory::token_list_t& values)
                {
                    return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_LESS_THAN);
                }),
            create.Type(CommandsParserValueType::LESS_EQUAL)
                .Transform(
                    [](CommandsMatcherFactory::token_list_t& values)
                    {
                        return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_LESS_EQUAL_THAN);
                    }),
            create.Type(CommandsParserValueType::EQUALS)
                .Transform(
                    [](CommandsMatcherFactory::token_list_t& values)
                    {
                        return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_EQUALS);
                    }),
            create.Type(CommandsParserValueType::NOT_EQUAL)
                .Transform(
                    [](CommandsMatcherFactory::token_list_t& values)
                    {
                        return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_NOT_EQUAL);
                    }),
            create.Type(CommandsParserValueType::LOGICAL_AND)
                .Transform(
                    [](CommandsMatcherFactory::token_list_t& values)
                    {
                        return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_AND);
                    }),
            create.Type(CommandsParserValueType::LOGICAL_OR)
                .Transform(
                    [](CommandsMatcherFactory::token_list_t& values)
                    {
                        return CommandsParserValue::OpType(values[0].get().GetPos(), OperationType::OPERATION_OR);
                    }),
        })
        .Capture(CAPTURE_BINARY_OPERATION_TYPE);
}

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::Evaluation(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create
        .And({
            create.Or({
                create
                    .And({
                        create.Optional(create.Char('!').Tag(TAG_EVALUATION_NOT)),
                        create.Char('('),
                        create.Label(LABEL_EVALUATION),
                        create.Char(')').Tag(TAG_EVALUATION_PARENTHESIS_END),
                    })
                    .Tag(TAG_EVALUATION_PARENTHESIS),
                ParseOperand(labelSupplier),
            }),
            create.Optional(create
                                .And({
                                    ParseOperationType(labelSupplier),
                                    create.Label(LABEL_EVALUATION),
                                })
                                .Tag(TAG_EVALUATION_OPERATION)),
        })
        .Tag(TAG_EVALUATION);
}

std::unique_ptr<IEvaluation> CommandsCommonMatchers::ProcessEvaluationInParenthesis(CommandsParserState* state, SequenceResult<CommandsParserValue>& result)
{
    const auto isNegated = result.PeekAndRemoveIfTag(TAG_EVALUATION_NOT) == TAG_EVALUATION_NOT;

    auto processedEvaluation = ProcessEvaluation(state, result);

    if (result.PeekAndRemoveIfTag(TAG_EVALUATION_PARENTHESIS_END) != TAG_EVALUATION_PARENTHESIS_END)
        throw ParsingException(TokenPos(), "Expected parenthesis end tag @ EvaluationInParenthesis");

    if (isNegated)
        processedEvaluation = std::make_unique<Operation>(OperationType::OPERATION_EQUALS, std::move(processedEvaluation), std::make_unique<OperandStatic>(0));

    return processedEvaluation;
}

std::unique_ptr<IEvaluation>
    CommandsCommonMatchers::ProcessOperand(CommandsParserState* state, SequenceResult<CommandsParserValue>& result, StructureInformation* currentType)
{
    const auto nextTag = result.NextTag();

    if (nextTag == TAG_OPERAND_INTEGER)
    {
        return std::make_unique<OperandStatic>(result.NextCapture(CAPTURE_OPERAND_INTEGER).IntegerValue());
    }

    if (nextTag == TAG_OPERAND_TYPENAME)
    {
        const auto& typeNameToken = result.NextCapture(CAPTURE_OPERAND_TYPENAME);
        std::vector<std::unique_ptr<IEvaluation>> arrayIndexEvaluations;

        while (result.PeekAndRemoveIfTag(TAG_OPERAND_ARRAY) == TAG_OPERAND_ARRAY)
        {
            arrayIndexEvaluations.emplace_back(ProcessEvaluation(state, result, currentType));

            if (result.PeekAndRemoveIfTag(TAG_OPERAND_ARRAY_END) != TAG_OPERAND_ARRAY_END)
                throw ParsingException(TokenPos(), "Expected operand array end tag @ Operand");
        }

        auto* foundEnumMember = state->GetRepository()->GetEnumMemberByName(typeNameToken.TypeNameValue());
        if (foundEnumMember != nullptr)
            return std::make_unique<OperandStatic>(foundEnumMember);

        StructureInformation* structure;
        std::vector<MemberInformation*> memberChain;
        if (state->GetTypenameAndMembersFromTypename(typeNameToken.TypeNameValue(), structure, memberChain))
            return std::make_unique<OperandDynamic>(structure, std::move(memberChain), std::move(arrayIndexEvaluations));

        if (currentType != nullptr && state->GetMembersFromTypename(typeNameToken.TypeNameValue(), currentType, memberChain))
            return std::make_unique<OperandDynamic>(currentType, std::move(memberChain), std::move(arrayIndexEvaluations));

        throw ParsingException(typeNameToken.GetPos(), "Unknown type");
    }

    throw ParsingException(TokenPos(), "Unknown operand type @ Operand");
}

std::unique_ptr<IEvaluation> CommandsCommonMatchers::ProcessEvaluation(CommandsParserState* state, SequenceResult<CommandsParserValue>& result)
{
    return ProcessEvaluation(state, result, nullptr);
}

std::unique_ptr<IEvaluation>
    CommandsCommonMatchers::ProcessEvaluation(CommandsParserState* state, SequenceResult<CommandsParserValue>& result, StructureInformation* currentType)
{
    if (result.PeekAndRemoveIfTag(TAG_EVALUATION) != TAG_EVALUATION)
        return nullptr;

    if (currentType == state->GetInUse())
        currentType = nullptr;

    std::vector<std::unique_ptr<IEvaluation>> operands;
    std::list<std::pair<unsigned, const OperationType*>> operators;

    while (true)
    {
        std::unique_ptr<IEvaluation> firstStatementPart;
        const auto nextTag = result.NextTag();
        switch (nextTag)
        {
        case TAG_EVALUATION_PARENTHESIS:
            firstStatementPart = ProcessEvaluationInParenthesis(state, result);
            break;

        case TAG_OPERAND:
            firstStatementPart = ProcessOperand(state, result, currentType);
            break;

        default:
            throw ParsingException(TokenPos(), "Invalid followup tag @ Evaluation");
        }
        operands.emplace_back(std::move(firstStatementPart));

        if (result.PeekAndRemoveIfTag(TAG_EVALUATION_OPERATION) == TAG_EVALUATION_OPERATION)
            operators.emplace_back(operators.size(), result.NextCapture(CAPTURE_BINARY_OPERATION_TYPE).OpTypeValue());
        else
            break;

        if (result.PeekAndRemoveIfTag(TAG_EVALUATION) != TAG_EVALUATION)
            throw ParsingException(TokenPos(), "Expected EvaluationTag @ Evaluation");
    }

    operators.sort(
        [](const std::pair<unsigned, const OperationType*>& p1, const std::pair<unsigned, const OperationType*>& p2)
        {
            if (p1.second->m_precedence != p2.second->m_precedence)
                return p1.second->m_precedence > p2.second->m_precedence;

            return p1.first > p2.first;
        });

    while (!operators.empty())
    {
        const auto [operatorIndex, operatorType] = operators.back();

        auto operation = std::make_unique<Operation>(operatorType, std::move(operands[operatorIndex]), std::move(operands[operatorIndex + 1]));
        operands.erase(operands.begin() + operatorIndex);
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
