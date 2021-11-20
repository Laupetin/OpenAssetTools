#include "MenuMatcherScriptNumeric.h"

MenuMatcherScriptNumeric::MenuMatcherScriptNumeric()
= default;

MatcherResult<SimpleParserValue> MenuMatcherScriptNumeric::CanMatch(ILexer<SimpleParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);

    if (token.m_type == SimpleParserValueType::FLOATING_POINT || token.m_type == SimpleParserValueType::INTEGER)
        return MatcherResult<SimpleParserValue>::Match(1);

    if (token.m_type != SimpleParserValueType::STRING)
        return MatcherResult<SimpleParserValue>::NoMatch();

    const auto& stringValue = token.StringValue();

    if (stringValue.empty())
        return MatcherResult<SimpleParserValue>::NoMatch();

    char* endPtr;
    // The return result does not matter here
    const auto _ = strtod(&stringValue[0], &endPtr);

    if(endPtr != &stringValue[stringValue.size()])
        return MatcherResult<SimpleParserValue>::NoMatch();

    return MatcherResult<SimpleParserValue>::Match(1);
}
