#pragma once

#include "HeaderParserValue.h"
#include "Parsing/Impl/AbstractLexer.h"

#include <string>
#include <unordered_map>

class HeaderLexer final : public AbstractLexer<HeaderParserValue>
{
public:
    explicit HeaderLexer(IParserLineStream* stream);

protected:
    HeaderParserValue GetNextToken() override;

private:
    void PrepareKeywords();

    std::unordered_map<std::string, HeaderParserValueType> m_keywords;
};
