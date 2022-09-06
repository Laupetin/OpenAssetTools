#include "SimpleExpressionInterpreter.h"

#include "Utils/ClassUtils.h"
#include "Expression/SimpleExpressionMatchers.h"
#include "Matcher/SimpleMatcherFactory.h"
#include "Parsing/Impl/AbstractParser.h"

class SimpleExpressionInterpreterParserState
{
public:
    std::unique_ptr<ISimpleExpression> m_expression;
};

class SimpleExpressionInterpreterExpressionSequence final : public AbstractSequence<SimpleParserValue, SimpleExpressionInterpreterParserState>
{
public:
    SimpleExpressionInterpreterExpressionSequence()
    {
        const SimpleMatcherFactory create(this);
        AddLabeledMatchers(m_expression_matchers.Expression(this), SimpleExpressionMatchers::LABEL_EXPRESSION);
        AddMatchers(create.Label(SimpleExpressionMatchers::LABEL_EXPRESSION));
    }

protected:
    void ProcessMatch(SimpleExpressionInterpreterParserState* state, SequenceResult<SimpleParserValue>& result) const override
    {
        state->m_expression = m_expression_matchers.ProcessExpression(result);
    }

private:
    SimpleExpressionMatchers m_expression_matchers;
};

class SimpleExpressionInterpreterParser final : public AbstractParser<SimpleParserValue, SimpleExpressionInterpreterParserState>
{
public:
    explicit SimpleExpressionInterpreterParser(ILexer<SimpleParserValue>* lexer)
        : AbstractParser<SimpleParserValue, SimpleExpressionInterpreterParserState>(lexer, std::make_unique<SimpleExpressionInterpreterParserState>())
    {
    }

    _NODISCARD std::unique_ptr<ISimpleExpression> GetParsedExpression() const
    {
        return std::move(m_state->m_expression);
    }

protected:
    const std::vector<sequence_t*>& GetTestsForState() override
    {
        static std::vector<sequence_t*> sequences
        {
            new SimpleExpressionInterpreterExpressionSequence()
        };
        return sequences;
    }
};

SimpleExpressionInterpreter::SimpleExpressionInterpreter(IParserLineStream* input)
    : m_input(input)
{
}

std::unique_ptr<ISimpleExpression> SimpleExpressionInterpreter::Evaluate() const
{
    SimpleLexer::Config lexerConfig;
    lexerConfig.m_emit_new_line_tokens = false;
    lexerConfig.m_read_integer_numbers = true;
    lexerConfig.m_read_floating_point_numbers = true;
    lexerConfig.m_read_strings = true;
    SimpleExpressionMatchers().ApplyTokensToLexerConfig(lexerConfig);

    SimpleLexer lexer(m_input, std::move(lexerConfig));
    SimpleExpressionInterpreterParser parser(&lexer);

    if (!parser.Parse())
        return nullptr;

    return parser.GetParsedExpression();
}
