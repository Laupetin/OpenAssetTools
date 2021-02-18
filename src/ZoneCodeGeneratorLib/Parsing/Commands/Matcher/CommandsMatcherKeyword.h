#pragma once

#include <string>

#include "Parsing/Commands/Impl/CommandsParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class CommandsMatcherKeyword final : public AbstractMatcher<CommandsParserValue>
{
    size_t m_hash;
    std::string m_value;

protected:
    MatcherResult<CommandsParserValue> CanMatch(ILexer<CommandsParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit CommandsMatcherKeyword(std::string value);
};
