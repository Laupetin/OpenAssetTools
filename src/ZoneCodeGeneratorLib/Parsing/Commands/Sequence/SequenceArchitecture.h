#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"

#include <unordered_map>

class SequenceArchitecture final : public CommandsParser::sequence_t
{
public:
    SequenceArchitecture();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

private:
    std::unordered_map<std::string, Architecture> m_architecture_mapping;
};
