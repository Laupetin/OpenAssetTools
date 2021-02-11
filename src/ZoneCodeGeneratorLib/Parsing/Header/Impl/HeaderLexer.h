#pragma once

#include <unordered_map>

#include "HeaderParserValue.h"
#include "Parsing/AbstractLexer.h"

class HeaderLexer final : public AbstractLexer<HeaderParserValue>
{
    std::unordered_map<std::string, HeaderParserValueType> m_keywords;

    void PrepareKeywords();

protected:
    HeaderParserValue GetNextToken() override;

public:
    explicit HeaderLexer(IParserLineStream* stream);
};
