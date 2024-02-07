#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceAssetRef final : public CommandsParser::sequence_t
{
    static constexpr auto TAG_DEFAULT = 1;

    static constexpr auto CAPTURE_TYPE = 1;
    static constexpr auto CAPTURE_ASSET_TYPE_ENUM_ENTRY = 2;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceAssetRef();
};
