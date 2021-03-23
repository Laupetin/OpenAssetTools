#pragma once

#include "Parsing/LocalizeFile/LocalizeFileParser.h"

class SequenceLocalizeFileReference final : public LocalizeFileParser::sequence_t
{
    static constexpr auto CAPTURE_REFERENCE_NAME = 1;

protected:
    void ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

public:
    SequenceLocalizeFileReference();
};
