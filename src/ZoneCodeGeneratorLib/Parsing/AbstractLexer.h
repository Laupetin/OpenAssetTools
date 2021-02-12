#pragma once

#include <cassert>
#include <deque>

#include "Utils/ClassUtils.h"
#include "ILexer.h"
#include "IParserValue.h"
#include "IParserLineStream.h"
#include "ParsingException.h"

template <typename TokenType>
class AbstractLexer : public ILexer
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

protected:
    std::deque<TokenType> m_token_cache;
    IParserLineStream* const m_stream;

    unsigned m_current_line_offset;
    bool m_peeked_next_line;
    bool m_start;
    ParserLine m_current_line;
    ParserLine m_next_line;

    explicit AbstractLexer(IParserLineStream* stream)
        : m_stream(stream),
          m_current_line_offset(0u),
          m_peeked_next_line(false),
          m_start(true)
    {
    }

    virtual TokenType GetNextToken() = 0;

    int NextChar()
    {
        if (m_current_line.IsEof())
        {
            if (m_start)
                m_start = false;
            else
                return EOF;
        }

        while (m_current_line_offset >= m_current_line.m_line.size())
        {
            m_current_line_offset = 0;
            if (m_peeked_next_line)
            {
                m_current_line = m_next_line;
                m_peeked_next_line = false;
            }
            else
            {
                m_current_line = m_stream->NextLine();
            }

            if (m_current_line.IsEof())
                return EOF;
        }

        return m_current_line.m_line[m_current_line_offset++];
    }

    int PeekChar()
    {
        if (m_current_line.IsEof())
            return EOF;

        if (m_current_line_offset >= m_current_line.m_line.size())
        {
            m_peeked_next_line = true;

            do
            {
                m_next_line = m_stream->NextLine();
                if (m_next_line.IsEof())
                    return EOF;
            }
            while (m_next_line.m_line.empty());

            return m_next_line.m_line[0];
        }

        return m_current_line.m_line[m_current_line_offset];
    }

    _NODISCARD bool IsLineEnd() const
    {
        return m_current_line_offset >= m_current_line.m_line.size();
    }

    _NODISCARD bool NextCharInLineIs(const char c)
    {
        return !IsLineEnd() && PeekChar() == c;
    }

    _NODISCARD TokenPos GetPreviousCharacterPos() const
    {
        return TokenPos(m_current_line.m_filename, m_current_line.m_line_number, m_current_line_offset);
    }

    _NODISCARD TokenPos GetNextCharacterPos()
    {
        if (m_current_line_offset + 1 >= m_current_line.m_line.size())
        {
            PeekChar();
            return TokenPos(m_next_line.m_filename, m_next_line.m_line_number, 1);
        }

        return TokenPos(m_current_line.m_filename, m_current_line.m_line_number, m_current_line_offset + 1);
    }

    /**
     * \brief Reads an identifier from the current position
     * \return The value of the read identifier
     */
    std::string ReadIdentifier()
    {
        assert(m_current_line_offset >= 1);
        assert(isalpha(m_current_line.m_line[m_current_line_offset - 1]) || m_current_line.m_line[m_current_line_offset - 1] == '_');

        const auto startPos = m_current_line_offset - 1;
        const auto lineSize = m_current_line.m_line.size();
        while (m_current_line_offset < lineSize)
        {
            const auto c = m_current_line.m_line[m_current_line_offset];

            if (!isalnum(c) && c != '_')
                break;

            m_current_line_offset++;
        }

        return std::string(m_current_line.m_line, startPos, m_current_line_offset - startPos);
    }

    /**
     * \brief Reads an identifier from the current position
     * \return The value of the read identifier
     */
    std::string ReadString()
    {
        assert(m_current_line_offset >= 1);
        assert(m_current_line.m_line[m_current_line_offset - 1] == '"');

        const auto startPos = m_current_line_offset;
        const auto lineSize = m_current_line.m_line.size();
        while (true)
        {
            if (m_current_line_offset >= lineSize)
                throw ParsingException(TokenPos(m_current_line.m_filename, m_current_line.m_line_number, m_current_line_offset), "Unclosed string");

            if (m_current_line.m_line[m_current_line_offset] == '\"')
                break;

            m_current_line_offset++;
        }
        
        return std::string(m_current_line.m_line, startPos, m_current_line_offset++ - startPos);
    }

    void ReadHexNumber(int& integerValue)
    {
        const auto* start = &m_current_line.m_line.c_str()[m_current_line_offset - 1];
        char* end;

        integerValue = static_cast<int>(std::strtoul(start, &end, 16));
        const auto numberLength = static_cast<unsigned>(end - start);
        if (numberLength == 0 || isalnum(*end) || *end == '_')
            throw ParsingException(GetPreviousCharacterPos(), "Invalid hex number");

        m_current_line_offset += numberLength;
    }

    _NODISCARD bool IsIntegerNumber() const
    {
        const auto* currentCharacter = &m_current_line.m_line.c_str()[m_current_line_offset - 1];
        auto isInteger = true;
        auto dot = false;
        auto exponent = false;

        if (*currentCharacter == '-' || *currentCharacter == '+')
            currentCharacter++;

        while (*currentCharacter)
        {
            if (isdigit(*currentCharacter))
            {
            }
            else if (*currentCharacter == '.')
            {
                if (dot)
                    throw ParsingException(GetPreviousCharacterPos(), "Invalid number");
                dot = true;
                isInteger = false;
            }
            else if (*currentCharacter == 'e' || *currentCharacter == 'E')
            {
                if (exponent)
                    throw ParsingException(GetPreviousCharacterPos(), "Invalid number");
                if (currentCharacter[1] == '-')
                    currentCharacter++;
                exponent = true;
                isInteger = false;
            }
            else if (isalpha(*currentCharacter))
            {
                throw ParsingException(GetPreviousCharacterPos(), "Invalid number");
            }
            else
            {
                break;
            }

            currentCharacter++;
        }

        return isInteger;
    }

    int ReadInteger()
    {
        const auto* start = &m_current_line.m_line.c_str()[m_current_line_offset - 1];
        char* end;
        const auto integerValue = std::strtol(start, &end, 10);
        const auto numberLength = static_cast<unsigned>(end - start);

        if(numberLength == 0)
            throw ParsingException(GetPreviousCharacterPos(), "Invalid number");

        m_current_line_offset += numberLength - 1;

        return integerValue;
    }

    double ReadFloatingPoint()
    {
        const auto* start = &m_current_line.m_line.c_str()[m_current_line_offset - 1];
        char* end;
        const auto floatingPointValue = std::strtod(start, &end);
        const auto numberLength = static_cast<unsigned>(end - start);

        if (numberLength == 0)
            throw ParsingException(GetPreviousCharacterPos(), "Invalid number");

        m_current_line_offset += numberLength - 1;

        return floatingPointValue;
    }

    void ReadNumber(bool& isFloatingPoint, double& floatingPointValue, int& integerValue)
    {
        assert(m_current_line_offset >= 1);
        assert(isdigit(m_current_line.m_line[m_current_line_offset - 1]));

        const auto lineLength = m_current_line.m_line.size();
        if (lineLength - m_current_line_offset >= 1
            && m_current_line.m_line[m_current_line_offset - 1] == '0'
            && m_current_line.m_line[m_current_line_offset] == 'x')
        {
            isFloatingPoint = false;
            ReadHexNumber(integerValue);
        }
        else if (IsIntegerNumber())
        {
            isFloatingPoint = false;
            integerValue = ReadInteger();
        }
        else
        {
            isFloatingPoint = true;
            floatingPointValue = ReadFloatingPoint();
        }
    }

public:
    const TokenType& GetToken(unsigned index)
    {
        assert(index >= 0);
        while (index >= m_token_cache.size())
            m_token_cache.emplace_back(GetNextToken());

        return m_token_cache[index];
    }

    void PopTokens(int amount) override
    {
        m_token_cache.erase(m_token_cache.begin(), m_token_cache.begin() + amount);
    }

    _NODISCARD bool IsEof()
    {
        return GetToken(0).IsEof();
    }

    _NODISCARD const TokenPos& GetPos()
    {
        return GetToken(0).GetPos();
    }
};
