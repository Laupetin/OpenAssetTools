#pragma once

#include "Parsing/Commands/Impl/CommandsParserValue.h"
#include "Parsing/Matcher/AbstractMatcher.h"

class CommandsMatcherValueType final : public AbstractMatcher<CommandsParserValue>
{
    CommandsParserValueType m_type;

protected:
    MatcherResult<CommandsParserValue> CanMatch(ILexer<CommandsParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit CommandsMatcherValueType(CommandsParserValueType type);
};
