#include "SequenceLocalizeFileConfig.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceLocalizeFileConfig::SequenceLocalizeFileConfig()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("CONFIG"),
        create.String(),
        create.Type(SimpleParserValueType::NEW_LINE),
    });
}

void SequenceLocalizeFileConfig::ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const {}
