#pragma once

#include "SimpleParserValue.h"
#include "Parsing/Impl/AbstractLexer.h"

class SimpleLexer : public AbstractLexer<SimpleParserValue>
{
public:
    class Config
    {
    public:
        bool m_emit_new_line_tokens;
        bool m_read_strings;
        bool m_read_numbers;
    };

    Config m_config;
    int m_last_line;

protected:
    SimpleParserValue GetNextToken() override;

public:
    explicit SimpleLexer(IParserLineStream* stream);
    SimpleLexer(IParserLineStream* stream, Config config);
};