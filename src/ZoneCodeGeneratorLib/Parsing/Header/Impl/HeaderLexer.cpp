#include "HeaderLexer.h"

HeaderLexer::HeaderLexer(IParserLineStream* stream)
    : AbstractLexer(stream)
{
}

HeaderParserValue HeaderLexer::GetNextToken()
{
    auto c = NextChar();

    while (c != EOF)
    {
        switch (c)
        {
        case '\"':
            {
                return HeaderParserValue::String(GetPreviousCharacterPos(), new std::string(ReadString()));
            }

        case '<':
            {
                if (!IsLineEnd())
                {
                    const auto pos = GetPreviousCharacterPos();
                    const auto nextChar = PeekChar();

                    if (nextChar == '=')
                    {
                        NextChar();
                        return HeaderParserValue::LessEqual(pos);
                    }
                    if (nextChar == '<')
                    {
                        NextChar();
                        return HeaderParserValue::ShiftLeft(pos);
                    }
                }

                return HeaderParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
            }

        case '>':
            {
                if (!IsLineEnd())
                {
                    const auto pos = GetPreviousCharacterPos();
                    const auto nextChar = PeekChar();

                    if (nextChar == '=')
                    {
                        NextChar();
                        return HeaderParserValue::GreaterEqual(pos);
                    }
                    if (nextChar == '>')
                    {
                        NextChar();
                        return HeaderParserValue::ShiftRight(pos);
                    }
                }

                return HeaderParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
            }

        case '=':
            {
                if (NextCharInLineIs('='))
                {
                    const auto pos = GetPreviousCharacterPos();
                    NextChar();
                    return HeaderParserValue::Equals(pos);
                }

                return HeaderParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
            }

        case '&':
            {
                if (NextCharInLineIs('&'))
                {
                    const auto pos = GetPreviousCharacterPos();
                    NextChar();
                    return HeaderParserValue::LogicalAnd(pos);
                }

                return HeaderParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
            }

        case '|':
            {
                if (NextCharInLineIs('|'))
                {
                    const auto pos = GetPreviousCharacterPos();
                    NextChar();
                    return HeaderParserValue::LogicalOr(pos);
                }

                return HeaderParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
            }

        case '!':
            {
                if (NextCharInLineIs('='))
                {
                    const auto pos = GetPreviousCharacterPos();
                    NextChar();
                    return HeaderParserValue::NotEqual(pos);
                }

                return HeaderParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
            }

        default:
            {
                if (isspace(c))
                    break;

                if(isdigit(c))
                {
                    const auto pos = GetPreviousCharacterPos();
                    bool isFloatingPointValue;
                    double doubleValue;
                    int integerValue;

                    ReadNumber(isFloatingPointValue, doubleValue, integerValue);

                    if (isFloatingPointValue)
                        return HeaderParserValue::FloatingPoint(pos, doubleValue);

                    return HeaderParserValue::Integer(pos, integerValue);
                }

                if (isalpha(c) || c == '_')
                    return HeaderParserValue::Identifier(GetPreviousCharacterPos(), new std::string(ReadIdentifier()));

                return HeaderParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
            }
        }

        c = NextChar();
    }

    return HeaderParserValue::EndOfFile(TokenPos());
}
