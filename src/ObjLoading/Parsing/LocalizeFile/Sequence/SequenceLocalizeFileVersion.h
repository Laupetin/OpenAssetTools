#pragma once

#include "Parsing/LocalizeFile/LocalizeFileParser.h"

class SequenceLocalizeFileVersion final : public LocalizeFileParser::sequence_t
{
    static constexpr auto CAPTURE_VERSION = 1;

protected:
    void ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

public:
    SequenceLocalizeFileVersion();
};
