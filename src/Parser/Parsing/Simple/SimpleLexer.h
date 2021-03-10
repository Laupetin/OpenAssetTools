#pragma once

#include "SimpleParserValue.h"
#include "Parsing/Impl/AbstractLexer.h"

class SimpleLexer : public AbstractLexer<SimpleParserValue>
{
    bool m_emit_new_line_tokens;
    bool m_read_strings;
    bool m_read_numbers;
    int m_last_line;

protected:
    SimpleParserValue GetNextToken() override;

    void SetShouldEmitNewLineTokens(bool value);
    void SetShouldReadStrings(bool value);
    void SetShouldReadNumbers(bool value);

public:
    explicit SimpleLexer(IParserLineStream* stream);
};