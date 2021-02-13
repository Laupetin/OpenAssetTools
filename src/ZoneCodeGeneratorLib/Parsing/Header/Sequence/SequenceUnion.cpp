#include "SequenceUnion.h"

#include "Parsing/Header/Matcher/HeaderMatcherFactory.h"
#include "Parsing/Header/Matcher/HeaderCommonMatchers.h"

SequenceUnion::SequenceUnion()
{
    const HeaderMatcherFactory create(this);

    AddLabeledMatchers(HeaderCommonMatchers::Align(this), HeaderCommonMatchers::LABEL_ALIGN);
    AddLabeledMatchers(HeaderCommonMatchers::Typename(this), HeaderCommonMatchers::LABEL_TYPENAME);
    AddMatchers({
        create.Optional(create.Type(HeaderParserValueType::TYPEDEF).Tag(TAG_TYPEDEF)),
        create.Optional(create.Type(HeaderParserValueType::CONST)),
        create.Type(HeaderParserValueType::UNION),
        create.Optional(create.Label(HeaderCommonMatchers::LABEL_ALIGN).Capture(CAPTURE_ALIGN)),
        create.Optional(create.Identifier().Capture(CAPTURE_NAME)),
        create.Optional(create.And({
            create.Char(':'),
            create.Label(HeaderCommonMatchers::LABEL_TYPENAME).Capture(CAPTURE_PARENT_TYPE)
        })),
        create.Char('{')
        });
}

void SequenceUnion::ProcessMatch(HeaderParserState* state, SequenceResult<HeaderParserValue>& result) const
{
}
