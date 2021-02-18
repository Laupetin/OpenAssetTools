#include "CommandsLexer.h"

CommandsLexer::CommandsLexer(IParserLineStream* stream)
    : AbstractLexer(stream)
{
}

CommandsParserValue CommandsLexer::GetNextToken()
{
    auto c = NextChar();

    while (c != EOF)
    {
        switch (c)
        {
        case '\"':
        {
            return CommandsParserValue::String(GetPreviousCharacterPos(), new std::string(ReadString()));
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
                    return CommandsParserValue::LessEqual(pos);
                }
                if (nextChar == '<')
                {
                    NextChar();
                    return CommandsParserValue::ShiftLeft(pos);
                }
            }

            return CommandsParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
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
                    return CommandsParserValue::GreaterEqual(pos);
                }
                if (nextChar == '>')
                {
                    NextChar();
                    return CommandsParserValue::ShiftRight(pos);
                }
            }

            return CommandsParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
        }

        case '=':
        {
            if (NextCharInLineIs('='))
            {
                const auto pos = GetPreviousCharacterPos();
                NextChar();
                return CommandsParserValue::Equals(pos);
            }

            return CommandsParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
        }

        case '&':
        {
            if (NextCharInLineIs('&'))
            {
                const auto pos = GetPreviousCharacterPos();
                NextChar();
                return CommandsParserValue::LogicalAnd(pos);
            }

            return CommandsParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
        }

        case '|':
        {
            if (NextCharInLineIs('|'))
            {
                const auto pos = GetPreviousCharacterPos();
                NextChar();
                return CommandsParserValue::LogicalOr(pos);
            }

            return CommandsParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
        }

        case '!':
        {
            if (NextCharInLineIs('='))
            {
                const auto pos = GetPreviousCharacterPos();
                NextChar();
                return CommandsParserValue::NotEqual(pos);
            }

            return CommandsParserValue::Character(GetPreviousCharacterPos(), static_cast<char>(c));
        }

        default:
        {
            if (isspace(c))
                break;

            const auto pos = GetPreviousCharacterPos();
            if (isdigit(c))
            {
                bool isFloatingPointValue;
                double doubleValue;
                int integerValue;

                ReadNumber(isFloatingPointValue, doubleValue, integerValue);

                if (isFloatingPointValue)
                    return CommandsParserValue::FloatingPoint(pos, doubleValue);

                return CommandsParserValue::Integer(pos, integerValue);
            }

            if (isalpha(c) || c == '_')
            {
                auto identifier = ReadIdentifier();

                return CommandsParserValue::Identifier(pos, new std::string(std::move(identifier)));
            }

            return CommandsParserValue::Character(pos, static_cast<char>(c));
        }
        }

        c = NextChar();
    }

    return CommandsParserValue::EndOfFile(TokenPos());
}
