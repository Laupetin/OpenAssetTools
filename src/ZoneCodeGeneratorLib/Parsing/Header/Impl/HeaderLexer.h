#pragma once

#include "HeaderParserValue.h"
#include "Parsing/Impl/AbstractLexer.h"

#include <unordered_map>

class HeaderLexer final : public AbstractLexer<HeaderParserValue>
{
    std::unordered_map<std::string, HeaderParserValueType> m_keywords;

    void PrepareKeywords();

protected:
    HeaderParserValue GetNextToken() override;

public:
    explicit HeaderLexer(IParserLineStream* stream);
};
