#include "MenuMatcherScriptInt.h"

MenuMatcherScriptInt::MenuMatcherScriptInt() = default;

MatcherResult<SimpleParserValue> MenuMatcherScriptInt::CanMatch(ILexer<SimpleParserValue>* lexer, const unsigned tokenOffset)
{
    const auto& token = lexer->GetToken(tokenOffset);

    if (token.m_type == SimpleParserValueType::CHARACTER)
    {
        if (token.CharacterValue() != '-')
            return MatcherResult<SimpleParserValue>::NoMatch();

        const auto& nextToken = lexer->GetToken(tokenOffset + 1);
        if (nextToken.m_type != SimpleParserValueType::INTEGER)
            return MatcherResult<SimpleParserValue>::NoMatch();

        return MatcherResult<SimpleParserValue>::Match(2);
    }

    if (token.m_type == SimpleParserValueType::INTEGER)
        return MatcherResult<SimpleParserValue>::Match(1);

    if (token.m_type != SimpleParserValueType::STRING)
        return MatcherResult<SimpleParserValue>::NoMatch();

    const auto& stringValue = token.StringValue();

    if (stringValue.empty())
        return MatcherResult<SimpleParserValue>::NoMatch();

    char* endPtr;
    // The return result does not matter here
    const auto _ = strtol(stringValue.data(), &endPtr, 10);

    if (endPtr != &stringValue[stringValue.size() - 1])
        return MatcherResult<SimpleParserValue>::NoMatch();

    return MatcherResult<SimpleParserValue>::Match(1);
}
