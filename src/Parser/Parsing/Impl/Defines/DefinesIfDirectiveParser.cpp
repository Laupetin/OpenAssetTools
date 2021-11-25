#include "DefinesIfDirectiveParser.h"

#include "DefinesIfDirectiveExpressionSequence.h"

DefinesIfDirectiveParser::DefinesIfDirectiveParser(ILexer<SimpleParserValue>* lexer, const std::map<std::string, DefinesStreamProxy::Define>& defines)
    : AbstractParser<SimpleParserValue, DefinesIfDirectiveParsingState>(lexer, std::make_unique<DefinesIfDirectiveParsingState>(defines))
{
}

const std::vector<AbstractParser<SimpleParserValue, DefinesIfDirectiveParsingState>::sequence_t*>& DefinesIfDirectiveParser::GetTestsForState()
{
    static std::vector<sequence_t*> sequences
    {
        new DefinesIfDirectiveExpressionSequence()
    };
    return sequences;
}

std::unique_ptr<ISimpleExpression> DefinesIfDirectiveParser::GetParsedExpression() const
{
    return std::move(m_state->m_expression);
}
