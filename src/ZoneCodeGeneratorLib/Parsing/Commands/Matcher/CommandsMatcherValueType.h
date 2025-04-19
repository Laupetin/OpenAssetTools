#pragma once

#include "Parsing/Commands/Impl/CommandsParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class CommandsMatcherValueType final : public AbstractMatcher<CommandsParserValue>
{
public:
    explicit CommandsMatcherValueType(CommandsParserValueType type);

protected:
    MatcherResult<CommandsParserValue> CanMatch(ILexer<CommandsParserValue>* lexer, unsigned tokenOffset) override;

private:
    CommandsParserValueType m_type;
};
