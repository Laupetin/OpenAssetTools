#include "HeaderLexer.h"

HeaderLexer::HeaderLexer(IParserLineStream* stream)
    : AbstractLexer(stream)
{
    PrepareKeywords();
}

void HeaderLexer::PrepareKeywords()
{
    m_keywords["__declspec"] = HeaderParserValueType::DECLSPEC;
    m_keywords["align"] = HeaderParserValueType::ALIGN;
    m_keywords["alignas"] = HeaderParserValueType::ALIGNAS;
    m_keywords["char"] = HeaderParserValueType::CHAR;
    m_keywords["const"] = HeaderParserValueType::CONST;
    m_keywords["enum"] = HeaderParserValueType::ENUM;
    m_keywords["int"] = HeaderParserValueType::INT;
    m_keywords["long"] = HeaderParserValueType::LONG;
    m_keywords["namespace"] = HeaderParserValueType::NAMESPACE;
    m_keywords["short"] = HeaderParserValueType::SHORT;
    m_keywords["struct"] = HeaderParserValueType::STRUCT;
    m_keywords["typedef"] = HeaderParserValueType::TYPEDEF;
    m_keywords["union"] = HeaderParserValueType::UNION;
    m_keywords["unsigned"] = HeaderParserValueType::UNSIGNED;
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

                const auto pos = GetPreviousCharacterPos();
                if(isdigit(c))
                {
                    bool isFloatingPointValue;
                    double doubleValue;
                    int integerValue;

                    ReadNumber(isFloatingPointValue, doubleValue, integerValue);

                    if (isFloatingPointValue)
                        return HeaderParserValue::FloatingPoint(pos, doubleValue);

                    return HeaderParserValue::Integer(pos, integerValue);
                }

                if (isalpha(c) || c == '_')
                {
                    auto identifier = ReadIdentifier();

                    const auto foundKeyword = m_keywords.find(identifier);
                    if (foundKeyword != m_keywords.end())
                        return HeaderParserValue::Keyword(pos, foundKeyword->second);

                    return HeaderParserValue::Identifier(pos, new std::string(std::move(identifier)));
                }

                return HeaderParserValue::Character(pos, static_cast<char>(c));
            }
        }

        c = NextChar();
    }

    return HeaderParserValue::EndOfFile(TokenPos());
}
