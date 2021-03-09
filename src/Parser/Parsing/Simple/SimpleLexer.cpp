#include "SimpleLexer.h"

SimpleLexer::SimpleLexer(IParserLineStream* stream)
    : AbstractLexer(stream)
{
}

SimpleParserValue SimpleLexer::GetNextToken()
{
    auto c = NextChar();

    while (isspace(c))
        c = NextChar();

    if(c == EOF)
        return SimpleParserValue::EndOfFile(TokenPos());

    if (c == '\"')
        return SimpleParserValue::String(GetPreviousCharacterPos(), new std::string(ReadString()));

    const auto pos = GetPreviousCharacterPos();
    if (isdigit(c))
    {
        bool isFloatingPointValue;
        double doubleValue;
        int integerValue;

        ReadNumber(isFloatingPointValue, doubleValue, integerValue);

        if (isFloatingPointValue)
            return SimpleParserValue::FloatingPoint(pos, doubleValue);

        return SimpleParserValue::Integer(pos, integerValue);
    }

    if (isalpha(c) || c == '_')
    {
        auto identifier = ReadIdentifier();

        return SimpleParserValue::Identifier(pos, new std::string(std::move(identifier)));
    }

    return SimpleParserValue::Character(pos, static_cast<char>(c));
}
