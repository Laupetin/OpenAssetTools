#include <catch2/catch.hpp>

#include "Parsing/Impl/AbstractParser.h"
#include "Utils/ClassUtils.h"
#include "Parsing/Mock/MockLexer.h"
#include "Parsing/Simple/SimpleParserValue.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"
#include "Parsing/Simple/Expression/SimpleExpressionMatchers.h"
#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

namespace test::parsing::simple::expression
{
    class SimpleExpressionTestState
    {
    public:
        std::unique_ptr<ISimpleExpression> m_expression;
    };

    class SimpleExpressionSequence final : public AbstractSequence<SimpleParserValue, SimpleExpressionTestState>
    {
        const SimpleExpressionMatchers m_expression_matchers;

    public:
        SimpleExpressionSequence()
            : m_expression_matchers(true, true, true, true, true)
        {
            const SimpleMatcherFactory create(this);

            AddLabeledMatchers(m_expression_matchers.Expression(this), SimpleExpressionMatchers::LABEL_EXPRESSION);
            AddMatchers(create.Label(SimpleExpressionMatchers::LABEL_EXPRESSION));
        }

    protected:
        void ProcessMatch(SimpleExpressionTestState* state, SequenceResult<SimpleParserValue>& result) const override
        {
            state->m_expression = m_expression_matchers.ProcessExpression(result);
        }
    };

    class SimpleExpressionTestsHelper
    {
    public:
        std::unique_ptr<SimpleExpressionTestState> m_state;
        std::unique_ptr<ILexer<SimpleParserValue>> m_lexer;

        std::unique_ptr<SimpleExpressionSequence> m_sequence;

        unsigned m_consumed_token_count;

        explicit SimpleExpressionTestsHelper()
            : m_state(std::make_unique<SimpleExpressionTestState>()),
              m_sequence(std::make_unique<SimpleExpressionSequence>()),
              m_consumed_token_count(0u)
        {
        }

        void Tokens(std::initializer_list<Movable<SimpleParserValue>> tokens)
        {
            m_lexer = std::make_unique<MockLexer<SimpleParserValue>>(tokens, SimpleParserValue::EndOfFile(TokenPos()));
        }

        void Tokens(std::vector<SimpleParserValue> tokens)
        {
            m_lexer = std::make_unique<MockLexer<SimpleParserValue>>(std::move(tokens), SimpleParserValue::EndOfFile(TokenPos()));
        }

        bool PerformTest()
        {
            REQUIRE(m_lexer);

            m_consumed_token_count = 0;
            return m_sequence->MatchSequence(m_lexer.get(), m_state.get(), m_consumed_token_count);
        }
    };

    TEST_CASE("SimpleExpressions: Can parse expression with add operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 1336),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1337);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with subtract operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 428),
            SimpleParserValue::Character(pos, '-'),
            SimpleParserValue::Integer(pos, 8),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with multiply operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 105),
            SimpleParserValue::Character(pos, '*'),
            SimpleParserValue::Integer(pos, 4),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with divide operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 561540),
            SimpleParserValue::Character(pos, '/'),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1337);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with remainder operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 92673),
            SimpleParserValue::Character(pos, '%'),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with bitwise and operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 0x7FFFFFF0),
            SimpleParserValue::Character(pos, '&'),
            SimpleParserValue::Integer(pos, 0x2AAAAAAA),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 0x2AAAAAA0);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with bitwise or operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 5),
            SimpleParserValue::Character(pos, '|'),
            SimpleParserValue::Integer(pos, 3),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 7);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with shift left operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 105),
            SimpleParserValue::MultiCharacter(pos, SimpleExpressionMatchers::MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(SimpleBinaryOperationId::SHIFT_LEFT)),
            SimpleParserValue::Integer(pos, 2),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with shift right operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 42784),
            SimpleParserValue::MultiCharacter(pos, SimpleExpressionMatchers::MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(SimpleBinaryOperationId::SHIFT_RIGHT)),
            SimpleParserValue::Integer(pos, 5),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1337);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with greater operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::Character(pos, '>'),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with greater equal operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::MultiCharacter(pos, SimpleExpressionMatchers::MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(SimpleBinaryOperationId::GREATER_EQUAL_THAN)),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with less operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, '<'),
            SimpleParserValue::Integer(pos, 421),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with less equal operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::MultiCharacter(pos, SimpleExpressionMatchers::MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(SimpleBinaryOperationId::LESS_EQUAL_THAN)),
            SimpleParserValue::Integer(pos, 421),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with equals operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::MultiCharacter(pos, SimpleExpressionMatchers::MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(SimpleBinaryOperationId::EQUALS)),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with not equal operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::MultiCharacter(pos, SimpleExpressionMatchers::MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(SimpleBinaryOperationId::NOT_EQUAL)),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 0);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with and operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::MultiCharacter(pos, SimpleExpressionMatchers::MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(SimpleBinaryOperationId::AND)),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Can parse expression with or operation", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 0),
            SimpleParserValue::MultiCharacter(pos, SimpleExpressionMatchers::MULTI_TOKEN_OFFSET_BINARY + static_cast<int>(SimpleBinaryOperationId::OR)),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 3);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1337);
    }

    TEST_CASE("SimpleExpressions: Operator precedence is applied", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 100),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 4),
            SimpleParserValue::Character(pos, '*'),
            SimpleParserValue::Integer(pos, 25),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 220),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 7);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Parenthesis have highest precedence", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 40),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 2),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '*'),
            SimpleParserValue::Integer(pos, 10),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 7);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Simple conditional operator can be used with true value", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::Character(pos, '?'),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ':'),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 5);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Simple conditional operator can be used with false value", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 0),
            SimpleParserValue::Character(pos, '?'),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ':'),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 5);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1337);
    }

    TEST_CASE("SimpleExpressions: Simple conditional operator can be used within parenthesis with true value", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::Character(pos, '?'),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ':'),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 9);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 421);
    }

    TEST_CASE("SimpleExpressions: Simple conditional operator can be used within parenthesis with false value", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 0),
            SimpleParserValue::Character(pos, '?'),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ':'),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 9);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1338);
    }

    TEST_CASE("SimpleExpressions: Simple conditional operator can have an expression as condition", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, -1),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 2),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, '?'),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ':'),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 9);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Simple conditional operator can have an expression as true value", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::Character(pos, '?'),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 210),
            SimpleParserValue::Character(pos, '*'),
            SimpleParserValue::Integer(pos, 2),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::Character(pos, ':'),
            SimpleParserValue::Integer(pos, 1337),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 9);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 420);
    }

    TEST_CASE("SimpleExpressions: Simple conditional operator can have an expression as false value", "[parsing][simple][expression]")
    {
        SimpleExpressionTestsHelper helper;
        const TokenPos pos;
        helper.Tokens({
            SimpleParserValue::Integer(pos, 0),
            SimpleParserValue::Character(pos, '?'),
            SimpleParserValue::Integer(pos, 420),
            SimpleParserValue::Character(pos, ':'),
            SimpleParserValue::Character(pos, '('),
            SimpleParserValue::Integer(pos, 1336),
            SimpleParserValue::Character(pos, '+'),
            SimpleParserValue::Integer(pos, 1),
            SimpleParserValue::Character(pos, ')'),
            SimpleParserValue::EndOfFile(pos)
        });

        const auto result = helper.PerformTest();

        REQUIRE(result);
        REQUIRE(helper.m_consumed_token_count == 9);

        const auto& expression = helper.m_state->m_expression;
        REQUIRE(expression->IsStatic());

        const auto value = expression->Evaluate();
        REQUIRE(value.m_type == SimpleExpressionValue::Type::INT);
        REQUIRE(value.m_int_value == 1337);
    }
}
