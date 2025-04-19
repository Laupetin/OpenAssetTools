#pragma once

#include "Parsing/Commands/Impl/CommandsParser.h"
#include "Utils/ClassUtils.h"

#include <unordered_map>

class SequenceBlock final : public CommandsParser::sequence_t
{
public:
    SequenceBlock();

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

private:
    void AddFastFileBlockToLookup(std::string name, FastFileBlockType type);
    [[nodiscard]] bool GetFastFileBlockNameByType(const std::string& name, FastFileBlockType& type) const;

    std::unordered_map<std::string, FastFileBlockType> m_type_lookup;
};
