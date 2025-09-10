#pragma once

#include "Parsing/ILexer.h"
#include "Parsing/IParser.h"
#include "Parsing/ParsingException.h"
#include "Parsing/Sequence/AbstractSequence.h"
#include "Utils/Logging/Log.h"

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
                        con::error(
                            "{} L{}:{} Could not parse expression:\n{}", pos.m_filename.get(), pos.m_line, pos.m_column, line.m_line.substr(pos.m_column - 1));
                    }
                    else
                    {
                        con::error("{} L{}:{} Could not parse expression.", pos.m_filename.get(), pos.m_line, pos.m_column);
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
                con::error("{}\n{}", e.FullMessage(), line.m_line.substr(pos.m_column - 1));
            }
            else
            {
                con::error(e.FullMessage());
            }

            return false;
        }

        return true;
    }
};
