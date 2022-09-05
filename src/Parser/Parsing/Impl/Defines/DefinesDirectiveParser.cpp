#include "DefinesDirectiveParser.h"

#include "DefinesDirectiveExpressionSequence.h"

DefinesDirectiveParser::DefinesDirectiveParser(ILexer<SimpleParserValue>* lexer, const std::map<std::string, DefinesStreamProxy::Define>& defines)
    : AbstractParser<SimpleParserValue, DefinesDirectiveParsingState>(lexer, std::make_unique<DefinesDirectiveParsingState>(defines))
{
}

const std::vector<AbstractParser<SimpleParserValue, DefinesDirectiveParsingState>::sequence_t*>& DefinesDirectiveParser::GetTestsForState()
{
    static std::vector<sequence_t*> sequences
    {
        new DefinesDirectiveExpressionSequence()
    };
    return sequences;
}

std::unique_ptr<ISimpleExpression> DefinesDirectiveParser::GetParsedExpression() const
{
    return std::move(m_state->m_expression);
}
