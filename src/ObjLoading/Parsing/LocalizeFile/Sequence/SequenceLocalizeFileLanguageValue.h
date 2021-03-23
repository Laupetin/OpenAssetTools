#pragma once

#include "Parsing/LocalizeFile/LocalizeFileParser.h"

class SequenceLocalizeFileLanguageValue final : public LocalizeFileParser::sequence_t
{
    static constexpr auto CAPTURE_LANGUAGE_NAME = 1;
    static constexpr auto CAPTURE_ENTRY_VALUE = 2;

    static std::string UnescapeValue(const std::string& value);

protected:
    void ProcessMatch(LocalizeFileParserState* state, SequenceResult<SimpleParserValue>& result) const override;

public:
    SequenceLocalizeFileLanguageValue();
};
