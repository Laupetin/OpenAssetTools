#pragma once

#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Simple/SimpleParserValue.h"

class SimpleMatcherMultiCharacter final : public AbstractMatcher<SimpleParserValue>
{
    int m_multi_character_sequence_id;

protected:
    MatcherResult<SimpleParserValue> CanMatch(ILexer<SimpleParserValue>* lexer, unsigned tokenOffset) override;

public:
    explicit SimpleMatcherMultiCharacter(int multiCharacterSequenceId);
};
