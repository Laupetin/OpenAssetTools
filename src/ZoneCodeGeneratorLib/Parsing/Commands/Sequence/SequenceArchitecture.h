#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceArchitecture final : public CommandsParser::sequence_t
{
    static constexpr auto CAPTURE_ARCHITECTURE = 1;

    std::unordered_map<std::string, Architecture> m_architecture_mapping;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceArchitecture();
};
