#pragma once

#include "Parsing/Commands/Impl/CommandsParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

#include <string>

class CommandsMatcherKeyword final : public AbstractMatcher<CommandsParserValue>
{
public:
    explicit CommandsMatcherKeyword(std::string value);

protected:
    MatcherResult<CommandsParserValue> CanMatch(ILexer<CommandsParserValue>* lexer, unsigned tokenOffset) override;

private:
    size_t m_hash;
    std::string m_value;
};
