#pragma once

#include "Parsing/ILexer.h"
#include "Parsing/IParser.h"
#include "Parsing/ParsingException.h"
#include "Parsing/Sequence/AbstractSequence.h"

#include <iostream>
#include <vector>

template<typename TokenType, typename ParserState> class AbstractParser : public IParser
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

public:
    typedef AbstractSequence<TokenType, ParserState> sequence_t;

protected:
    ILexer<TokenType>* m_lexer;
    std::unique_ptr<ParserState> m_state;

    explicit AbstractParser(ILexer<TokenType>* lexer, std::unique_ptr<ParserState> state)
        : m_lexer(lexer),
          m_state(std::move(state))
    {
    }

    virtual const std::vector<sequence_t*>& GetTestsForState() = 0;

public:
    ~AbstractParser() override = default;
    AbstractParser(const AbstractParser& other) = default;
    AbstractParser(AbstractParser&& other) noexcept = default;
    AbstractParser& operator=(const AbstractParser& other) = default;
    AbstractParser& operator=(AbstractParser&& other) noexcept = default;

    bool Parse() override
    {
        try
        {
            while (!m_lexer->IsEof())
            {
                auto testSuccessful = false;
                const auto& availableTests = GetTestsForState();

                for (const sequence_t* test : availableTests)
                {
                    unsigned consumedTokenCount;

                    if (test->MatchSequence(m_lexer, m_state.get(), consumedTokenCount))
                    {
                        m_lexer->PopTokens(consumedTokenCount);
                        testSuccessful = true;
                        break;
                    }
                }

                if (!testSuccessful)
                {
                    const TokenPos& pos = m_lexer->GetPos();
                    const ParserLine line = m_lexer->GetLineForPos(pos);

                    if (!line.IsEof())
                    {
                        std::cerr << "Error: " << pos.m_filename.get() << " L" << pos.m_line << ':' << pos.m_column << " Could not parse expression:\n"
                                  << line.m_line.substr(pos.m_column - 1) << std::endl;
                    }
                    else
                    {
                        std::cerr << "Error: " << pos.m_filename.get() << " L" << pos.m_line << ':' << pos.m_column << " Could not parse expression."
                                  << std::endl;
                    }
                    return false;
                }
            }
        }
        catch (const ParsingException& e)
        {
            const auto pos = e.Position();
            const auto line = m_lexer->GetLineForPos(pos);

            if (!line.IsEof() && line.m_line.size() > static_cast<unsigned>(pos.m_column - 1))
            {
                std::cerr << "Error: " << e.FullMessage() << "\n" << line.m_line.substr(pos.m_column - 1) << std::endl;
            }
            else
            {
                std::cerr << "Error: " << e.FullMessage() << std::endl;
            }

            return false;
        }

        return true;
    }
};
