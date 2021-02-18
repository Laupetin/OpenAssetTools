#pragma once

#include "Parsing/Commands/Impl/CommandsParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class CommandsMatcherCharacter final : public AbstractMatcher<CommandsParserValue>
{
    char m_char;

protected:
    MatcherResult<CommandsParserValue> CanMatch(ILexer<CommandsParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit CommandsMatcherCharacter(char c);
};
