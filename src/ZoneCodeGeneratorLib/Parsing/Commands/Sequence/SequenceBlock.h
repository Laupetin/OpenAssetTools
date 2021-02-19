#pragma once

#include <unordered_map>

#include "Utils/ClassUtils.h"
#include "Parsing/Commands/Impl/CommandsParser.h"

class SequenceBlock final : public CommandsParser::sequence_t
{
    static constexpr auto TAG_DEFAULT = 1;

    static constexpr auto CAPTURE_BLOCK_ENUM_ENTRY = 1;
    static constexpr auto CAPTURE_BLOCK_TYPE = 2;

    std::unordered_map<std::string, FastFileBlockType> m_type_lookup;

    void AddFastFileBlockToLookup(std::string name, FastFileBlockType type);
    _NODISCARD bool GetFastFileBlockNameByType(const std::string& name, FastFileBlockType& type) const;

protected:
    void ProcessMatch(CommandsParserState* state, SequenceResult<CommandsParserValue>& result) const override;

public:
    SequenceBlock();
};
