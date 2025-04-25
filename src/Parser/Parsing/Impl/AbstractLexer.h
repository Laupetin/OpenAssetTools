#pragma once

#include "Parsing/ILexer.h"
#include "Parsing/IParserLineStream.h"
#include "Parsing/ParsingException.h"
#include "Utils/ClassUtils.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <concepts>
#include <deque>
#include <sstream>

template<std::derived_from<IParserValue> TokenType> class AbstractLexer : public ILexer<TokenType>
{
public:
    const TokenType& GetToken(const size_t index) override
    {
        while (index >= m_token_cache.size())
            m_token_cache.emplace_back(GetNextToken());

        return m_token_cache[index];
    }

    void PopTokens(size_t amount) override
    {
        if (amount == 0 || m_token_cache.empty())
            return;

        if (m_token_cache.size() <= amount)
        {
            const auto& lastToken = m_token_cache.back();
            while (
                !m_line_cache.empty()
                && (m_line_cache.front().m_line_number != lastToken.GetPos().m_line || *m_line_cache.front().m_filename != lastToken.GetPos().m_filename.get()))
            {
                m_line_cache.pop_front();
                m_line_index--;
            }
            m_token_cache.clear();
        }
        else
        {
            m_token_cache.erase(m_token_cache.begin(), m_token_cache.begin() + amount);
            const auto& firstToken = m_token_cache.front();
            while (!m_line_cache.empty()
                   && (m_line_cache.front().m_line_number != firstToken.GetPos().m_line
                       || *m_line_cache.front().m_filename != firstToken.GetPos().m_filename.get()))
            {
                m_line_cache.pop_front();
                m_line_index--;
            }
        }
    }

    [[nodiscard]] bool IsEof() override
    {
        return GetToken(0).IsEof();
    }

    [[nodiscard]] const TokenPos& GetPos() override
    {
        return GetToken(0).GetPos();
    }

    [[nodiscard]] ParserLine GetLineForPos(const TokenPos& pos) const override
    {
        for (const auto& line : m_line_cache)
        {
            if (line.m_filename && *line.m_filename == pos.m_filename.get() && line.m_line_number == pos.m_line)
            {
                return line;
            }
        }

        return ParserLine();
    }

protected:
    std::deque<TokenType> m_token_cache;
    std::deque<ParserLine> m_line_cache;
    IParserLineStream* m_stream;

    size_t m_line_index;
    size_t m_current_line_offset;

    explicit AbstractLexer(IParserLineStream* stream)
        : m_stream(stream),
          m_line_index(0u),
          m_current_line_offset(0u)
    {
    }

    virtual TokenType GetNextToken() = 0;

    int NextChar()
    {
        while (true)
        {
            while (m_line_index >= m_line_cache.size())
            {
                if (!m_line_cache.empty() && m_line_cache.back().IsEof())
                    return EOF;

                m_line_cache.push_back(m_stream->NextLine());
            }

            if (m_current_line_offset >= m_line_cache[m_line_index].m_line.size())
            {
                m_line_index++;
                m_current_line_offset = 0;
            }
            else
                break;
        }

        return m_line_cache[m_line_index].m_line[m_current_line_offset++];
    }

    int PeekChar()
    {
        auto peekLineOffset = m_current_line_offset;
        auto peekLine = m_line_index;

        while (true)
        {
            while (peekLine >= m_line_cache.size())
            {
                if (!m_line_cache.empty() && m_line_cache.back().IsEof())
                    return EOF;

                m_line_cache.push_back(m_stream->NextLine());
            }

            if (peekLineOffset >= m_line_cache[peekLine].m_line.size())
            {
                peekLine++;
                peekLineOffset = 0;
            }
            else
                break;
        }

        return m_line_cache[peekLine].m_line[peekLineOffset];
    }

    [[nodiscard]] const ParserLine& CurrentLine() const
    {
        return m_line_cache[m_line_index];
    }

    [[nodiscard]] bool IsLineEnd() const
    {
        return m_current_line_offset >= CurrentLine().m_line.size();
    }

    [[nodiscard]] bool NextCharInLineIs(const char c)
    {
        return !IsLineEnd() && PeekChar() == c;
    }

    [[nodiscard]] TokenPos GetPreviousCharacterPos() const
    {
        const auto& currentLine = CurrentLine();
        return TokenPos(*currentLine.m_filename, currentLine.m_line_number, m_current_line_offset);
    }

    [[nodiscard]] TokenPos GetNextCharacterPos()
    {
        const auto& currentLine = CurrentLine();
        if (m_current_line_offset + 1 >= currentLine.m_line.size())
        {
            PeekChar();
            return TokenPos(*m_line_cache.back().m_filename, m_line_cache.back().m_line_number, 1);
        }

        return TokenPos(*currentLine.m_filename, currentLine.m_line_number, m_current_line_offset + 1);
    }

    /**
     * \brief Reads an identifier from the current position
     * \return The value of the read identifier
     */
    std::string ReadIdentifier()
    {
        const auto& currentLine = CurrentLine();
        assert(m_current_line_offset >= 1);
        assert(isalpha(currentLine.m_line[m_current_line_offset - 1]) || currentLine.m_line[m_current_line_offset - 1] == '_');

        const auto startPos = m_current_line_offset - 1;
        const auto lineSize = currentLine.m_line.size();
        while (m_current_line_offset < lineSize)
        {
            const auto c = currentLine.m_line[m_current_line_offset];

            if (!isalnum(c) && c != '_')
                break;

            m_current_line_offset++;
        }

        return std::string(currentLine.m_line, startPos, m_current_line_offset - startPos);
    }

    /**
     * \brief Reads an identifier from the current position
     * \return The value of the read identifier
     */
    std::string ReadStringWithEscapeSequences()
    {
        const auto& currentLine = CurrentLine();
        assert(m_current_line_offset >= 1);
        assert(currentLine.m_line[m_current_line_offset - 1] == '"');

        const auto startPos = m_current_line_offset;
        const auto lineSize = currentLine.m_line.size();
        auto isEscaped = false;
        auto inEscape = false;
        while (true)
        {
            if (m_current_line_offset >= lineSize)
                throw ParsingException(TokenPos(*currentLine.m_filename, currentLine.m_line_number, m_current_line_offset), "Unclosed string");

            const auto c = currentLine.m_line[m_current_line_offset];

            if (c == '\"' && !inEscape)
                break;

            if (c == '\\' && !inEscape)
            {
                isEscaped = true;
                inEscape = true;
            }
            else
            {
                inEscape = false;
            }

            m_current_line_offset++;
        }

        std::string str(currentLine.m_line, startPos, m_current_line_offset++ - startPos);
        if (!isEscaped)
            return str;

        std::ostringstream ss;
        utils::UnescapeStringFromQuotationMarks(ss, std::move(str));
        return ss.str();
    }

    /**
     * \brief Reads an identifier from the current position
     * \return The value of the read identifier
     */
    std::string ReadString()
    {
        const auto& currentLine = CurrentLine();
        assert(m_current_line_offset >= 1);
        assert(currentLine.m_line[m_current_line_offset - 1] == '"');

        const auto startPos = m_current_line_offset;
        const auto lineSize = currentLine.m_line.size();
        while (true)
        {
            if (m_current_line_offset >= lineSize)
                throw ParsingException(TokenPos(*currentLine.m_filename, currentLine.m_line_number, m_current_line_offset), "Unclosed string");

            if (currentLine.m_line[m_current_line_offset] == '\"')
                break;

            m_current_line_offset++;
        }

        return std::string(currentLine.m_line, startPos, m_current_line_offset++ - startPos);
    }

    void ReadHexNumber(int& integerValue)
    {
        const auto& currentLine = CurrentLine();
        const auto* start = &currentLine.m_line.c_str()[m_current_line_offset - 1];
        char* end;

        integerValue = static_cast<int>(std::strtoul(start, &end, 16));
        const auto numberLength = static_cast<unsigned>(end - start);
        if (numberLength == 0 || isalnum(*end) || *end == '_')
            throw ParsingException(GetPreviousCharacterPos(), "Invalid hex number");

        m_current_line_offset += numberLength - 1;
    }

    [[nodiscard]] bool IsIntegerNumber() const
    {
        const auto& currentLine = CurrentLine();
        const auto* currentCharacter = &currentLine.m_line.c_str()[m_current_line_offset - 1];
        auto isInteger = true;
        auto dot = false;
        auto exponent = false;

        if (*currentCharacter == '-' || *currentCharacter == '+')
            ++currentCharacter;

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
                    ++currentCharacter;
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

            ++currentCharacter;
        }

        return isInteger;
    }

    int ReadInteger()
    {
        const auto& currentLine = CurrentLine();
        const auto* start = &currentLine.m_line.c_str()[m_current_line_offset - 1];
        char* end;
        const auto integerValue = std::strtol(start, &end, 10);
        const auto numberLength = static_cast<unsigned>(end - start);

        if (numberLength == 0)
            throw ParsingException(GetPreviousCharacterPos(), "Invalid number");

        m_current_line_offset += numberLength - 1;

        return integerValue;
    }

    double ReadFloatingPoint()
    {
        const auto& currentLine = CurrentLine();
        const auto* start = &currentLine.m_line.c_str()[m_current_line_offset - 1];
        char* end;
        const auto floatingPointValue = std::strtod(start, &end);
        const auto numberLength = static_cast<unsigned>(end - start);

        if (numberLength == 0)
            throw ParsingException(GetPreviousCharacterPos(), "Invalid number");

        m_current_line_offset += numberLength - 1;

        return floatingPointValue;
    }

    void ReadNumber(bool& isFloatingPoint, bool& hasSignPrefix, double& floatingPointValue, int& integerValue)
    {
        const auto& currentLine = CurrentLine();
        assert(m_current_line_offset >= 1);
        assert(isdigit(currentLine.m_line[m_current_line_offset - 1]) || currentLine.m_line[m_current_line_offset - 1] == '.'
               || currentLine.m_line[m_current_line_offset - 1] == '+' || currentLine.m_line[m_current_line_offset - 1] == '-');
        hasSignPrefix = currentLine.m_line[m_current_line_offset - 1] == '+' || currentLine.m_line[m_current_line_offset - 1] == '-';

        const auto lineLength = currentLine.m_line.size();
        if (lineLength - m_current_line_offset >= 1 && currentLine.m_line[m_current_line_offset - 1] == '0' && currentLine.m_line[m_current_line_offset] == 'x')
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

    void ReadNumber(bool& hasSignPrefix, int& integerValue)
    {
        const auto& currentLine = CurrentLine();
        assert(m_current_line_offset >= 1);
        assert(isdigit(currentLine.m_line[m_current_line_offset - 1]) || currentLine.m_line[m_current_line_offset - 1] == '+'
               || currentLine.m_line[m_current_line_offset - 1] == '-');
        hasSignPrefix = currentLine.m_line[m_current_line_offset - 1] == '+' || currentLine.m_line[m_current_line_offset - 1] == '-';

        const auto lineLength = currentLine.m_line.size();
        if (lineLength - m_current_line_offset >= 1 && currentLine.m_line[m_current_line_offset - 1] == '0' && currentLine.m_line[m_current_line_offset] == 'x')
        {
            ReadHexNumber(integerValue);
        }
        else
        {
            integerValue = ReadInteger();
        }
    }
};
