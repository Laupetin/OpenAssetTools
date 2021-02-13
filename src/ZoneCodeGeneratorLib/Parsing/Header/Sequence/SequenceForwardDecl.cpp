#include "SequenceForwardDecl.h"

#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"

SequenceForwardDecl::SequenceForwardDecl()
{
    const HeaderMatcherFactory create(this);

    AddMatchers({
        create.Or({
            create.Type(HeaderParserValueType::ENUM),
            create.Type(HeaderParserValueType::STRUCT),
            create.Type(HeaderParserValueType::UNION)
        }),
        create.Identifier().Capture(CAPTURE_NAME),
        create.Char(';')
    });
}

void SequenceForwardDecl::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
}
