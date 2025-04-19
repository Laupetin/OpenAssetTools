#pragma once

#include "Parsing/Commands/Impl/CommandsParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class CommandsMatcherCharacter final : public AbstractMatcher<CommandsParserValue>
{
public:
    explicit CommandsMatcherCharacter(char c);

protected:
    MatcherResult<CommandsParserValue> CanMatch(ILexer<CommandsParserValue>* lexer, unsigned tokenOffset) override;

private:
    char m_char;
};
