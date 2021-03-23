#pragma once

#include "Parsing/LocalizeFile/LocalizeFileParser.h"

class SequenceLocalizeFileEndMarker final : public LocalizeFileParser::sequence_t
{
protected:
    void ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

public:
    SequenceLocalizeFileEndMarker();
};
