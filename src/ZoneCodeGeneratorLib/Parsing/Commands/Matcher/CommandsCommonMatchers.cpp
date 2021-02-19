#include "CommandsCommonMatchers.h"

#include <sstream>

#include "CommandsMatcherFactory.h"

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::Typename(const supplier_t* labelSupplier)
{
    static constexpr const char* BUILT_IN_TYPE_NAMES[]
    {
        "unsigned",
        "char",
        "short",
        "int",
        "long"
    };
    static_assert(_countof(BUILT_IN_TYPE_NAMES) == static_cast<int>(CommandsParserValueType::BUILT_IN_LAST) - static_cast<int>(CommandsParserValueType::BUILT_IN_FIRST) + 1);

    const CommandsMatcherFactory create(labelSupplier);

    return create.Or({
        create.And({
            create.Optional(create.Type(CommandsParserValueType::UNSIGNED)),
            create.Or({
                create.Type(CommandsParserValueType::CHAR),
                create.Type(CommandsParserValueType::SHORT),
                create.Type(CommandsParserValueType::INT),
                create.And({
                    create.Type(CommandsParserValueType::LONG),
                    create.Optional(create.Type(CommandsParserValueType::LONG))
                })
            })
        }).Transform([](CommandsMatcherFactory::token_list_t& values)
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
        create.And({
            create.Identifier(),
            create.OptionalLoop(create.And({
                create.Char(':'),
                create.Char(':'),
                create.Identifier()
            }))
        }).Transform([](CommandsMatcherFactory::token_list_t& values)
        {
            std::ostringstream str;
            str << values[0].get().IdentifierValue();

            for (auto i = 3u; i < values.size(); i += 3)
                str << "::" << values[i].get().IdentifierValue();

            return CommandsParserValue::TypeName(values[0].get().GetPos(), new std::string(str.str()));
        })
    });
}

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::ArrayDef(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create.And({
        create.Char('['),
        create.Or({
            create.Integer(),
            create.Identifier()
        }),
        create.Char(']')
    }).Transform([](CommandsMatcherFactory::token_list_t& values)
    {
        if (values[1].get().m_type == CommandsParserValueType::INTEGER)
            return CommandsParserValue::Integer(values[1].get().GetPos(), values[1].get().IntegerValue());

        return CommandsParserValue::Identifier(values[1].get().GetPos(), new std::string(values[1].get().IdentifierValue()));
    });
}

static constexpr int TAG_OPERATION_TYPE = std::numeric_limits<int>::max() - 1;
static constexpr int TAG_ADD = std::numeric_limits<int>::max() - 2;
static constexpr int TAG_MINUS = std::numeric_limits<int>::max() - 3;
static constexpr int TAG_MULTIPLY = std::numeric_limits<int>::max() - 4;
static constexpr int TAG_DIVIDE = std::numeric_limits<int>::max() - 5;
static constexpr int TAG_REMAINDER = std::numeric_limits<int>::max() - 6;
static constexpr int TAG_BITWISE_AND = std::numeric_limits<int>::max() - 7;
static constexpr int TAG_BITWISE_XOR = std::numeric_limits<int>::max() - 8;
static constexpr int TAG_BITWISE_OR = std::numeric_limits<int>::max() - 9;
static constexpr int TAG_SHIFT_LEFT = std::numeric_limits<int>::max() - 10;
static constexpr int TAG_SHIFT_RIGHT = std::numeric_limits<int>::max() - 11;
static constexpr int TAG_GREATER_THAN = std::numeric_limits<int>::max() - 12;
static constexpr int TAG_GREATER_EQUAL = std::numeric_limits<int>::max() - 13;
static constexpr int TAG_LESS_THAN = std::numeric_limits<int>::max() - 14;
static constexpr int TAG_LESS_EQUAL = std::numeric_limits<int>::max() - 15;
static constexpr int TAG_EQUALS = std::numeric_limits<int>::max() - 16;
static constexpr int TAG_NOT_EQUAL = std::numeric_limits<int>::max() - 17;
static constexpr int TAG_LOGICAL_AND = std::numeric_limits<int>::max() - 18;
static constexpr int TAG_LOGICAL_OR = std::numeric_limits<int>::max() - 19;
static constexpr int TAG_OPERAND = std::numeric_limits<int>::max() - 20;
static constexpr int TAG_OPERAND_TYPENAME = std::numeric_limits<int>::max() - 21;
static constexpr int TAG_OPERAND_ARRAY = std::numeric_limits<int>::max() - 22;
static constexpr int TAG_OPERAND_ARRAY_END = std::numeric_limits<int>::max() - 23;
static constexpr int TAG_OPERAND_INTEGER = std::numeric_limits<int>::max() - 24;
static constexpr int TAG_OPERAND_FLOATING_POINT = std::numeric_limits<int>::max() - 25;
static constexpr int TAG_EVALUATION_NOT = std::numeric_limits<int>::max() - 26;
static constexpr int TAG_EVALUATION_PARENTHESIS = std::numeric_limits<int>::max() - 27;
static constexpr int TAG_EVALUATION_PARENTHESIS_END = std::numeric_limits<int>::max() - 28;
static constexpr int TAG_EVALUATION = std::numeric_limits<int>::max() - 29;
static constexpr int TAG_EVALUATION_OPERATION = std::numeric_limits<int>::max() - 30;

static constexpr int CAPTURE_OPERAND_TYPENAME = std::numeric_limits<int>::max() - 1;
static constexpr int CAPTURE_OPERAND_ARRAY = std::numeric_limits<int>::max() - 1;
static constexpr int CAPTURE_OPERAND_INTEGER = std::numeric_limits<int>::max() - 2;
static constexpr int CAPTURE_OPERAND_FLOATING_POINT = std::numeric_limits<int>::max() - 3;

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::OperandArray(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create.And({
        create.Char('['),
        create.Label(LABEL_EVALUATION),
        create.Char(']').Tag(TAG_OPERAND_ARRAY_END)
    }).Tag(TAG_OPERAND_ARRAY);
}

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::Operand(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create.Or({
        create.And({
            create.Label(LABEL_TYPENAME).Capture(CAPTURE_OPERAND_TYPENAME),
            create.OptionalLoop(MatcherFactoryWrapper<CommandsParserValue>(OperandArray(labelSupplier)).Capture(CAPTURE_OPERAND_ARRAY))
        }).Tag(TAG_OPERAND_TYPENAME),
        create.Integer().Tag(TAG_OPERAND_INTEGER).Capture(CAPTURE_OPERAND_INTEGER),
        create.FloatingPoint().Tag(TAG_OPERAND_FLOATING_POINT).Capture(CAPTURE_OPERAND_FLOATING_POINT)
    }).Tag(TAG_OPERAND);
}

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::OperationType(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create.Or({
        create.Char('+').Tag(TAG_ADD),
        create.Char('-').Tag(TAG_MINUS),
        create.Char('*').Tag(TAG_MULTIPLY),
        create.Char('/').Tag(TAG_DIVIDE),
        create.Char('%').Tag(TAG_REMAINDER),
        create.Char('&').Tag(TAG_BITWISE_AND),
        create.Char('^').Tag(TAG_BITWISE_XOR),
        create.Char('|').Tag(TAG_BITWISE_OR),
        create.Type(CommandsParserValueType::SHIFT_LEFT).Tag(TAG_SHIFT_LEFT),
        create.Type(CommandsParserValueType::SHIFT_RIGHT).Tag(TAG_SHIFT_RIGHT),
        create.Char('>').Tag(TAG_GREATER_THAN),
        create.Type(CommandsParserValueType::GREATER_EQUAL).Tag(TAG_GREATER_EQUAL),
        create.Char('<').Tag(TAG_LESS_THAN),
        create.Type(CommandsParserValueType::LESS_EQUAL).Tag(TAG_LESS_EQUAL),
        create.Type(CommandsParserValueType::EQUALS).Tag(TAG_EQUALS),
        create.Type(CommandsParserValueType::NOT_EQUAL).Tag(TAG_NOT_EQUAL),
        create.Type(CommandsParserValueType::LOGICAL_AND).Tag(TAG_LOGICAL_AND),
        create.Type(CommandsParserValueType::LOGICAL_OR).Tag(TAG_LOGICAL_OR)
    }).Tag(TAG_OPERATION_TYPE);
}

std::unique_ptr<CommandsCommonMatchers::matcher_t> CommandsCommonMatchers::Evaluation(const supplier_t* labelSupplier)
{
    const CommandsMatcherFactory create(labelSupplier);

    return create.And({
        create.Or({
            create.And({
                create.Optional(create.Char('!').Tag(TAG_EVALUATION_NOT)),
                create.Char('('),
                create.Label(LABEL_EVALUATION),
                create.Char(')').Tag(TAG_EVALUATION_PARENTHESIS_END)
            }).Tag(TAG_EVALUATION_PARENTHESIS),
            Operand(labelSupplier)
        }),
        create.Optional(create.And({
            OperationType(labelSupplier),
            create.Label(LABEL_EVALUATION)
        })).Tag(TAG_EVALUATION_OPERATION)
    }).Tag(TAG_EVALUATION);
}

std::unique_ptr<IEvaluation> CommandsCommonMatchers::ParseEvaluation(CommandsParserState* state, SequenceResult<CommandsParserValue>& result)
{
    if (result.PeekAndRemoveIfTag(TAG_EVALUATION) != TAG_EVALUATION)
        return nullptr;



    return nullptr;
}
