#pragma once

#include "Localize/Parsing/LocalizeFileParser.h"

class SequenceLocalizeFileConfig final : public LocalizeFileParser::sequence_t
{
protected:
    void ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

public:
    SequenceLocalizeFileConfig();
};
