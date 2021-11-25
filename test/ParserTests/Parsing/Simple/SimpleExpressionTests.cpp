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
            : m_expression_matchers(true, true, true, true)
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
    
}
