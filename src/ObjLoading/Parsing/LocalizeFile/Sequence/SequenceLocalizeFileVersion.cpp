#include "SequenceLocalizeFileVersion.h"

#include "Parsing/Simple/Matcher/SimpleMatcherFactory.h"

SequenceLocalizeFileVersion::SequenceLocalizeFileVersion()
{
    const SimpleMatcherFactory create(this);

    AddMatchers({
        create.Keyword("VERSION"),
        create.String().Capture(CAPTURE_VERSION),
        create.Type(SimpleParserValueType::NEW_LINE),
    });
}

void SequenceLocalizeFileVersion::ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const
{
    const auto& versionCapture = result.NextCapture(CAPTURE_VERSION);
    if (versionCapture.StringValue() != "1")
    {
        throw ParsingException(versionCapture.GetPos(), "Localize file needs to be version 1");
    }
}
