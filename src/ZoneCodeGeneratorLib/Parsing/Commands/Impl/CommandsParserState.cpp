#include "CommandsParserState.h"

CommandsParserState::CommandsParserState(IDataRepository* repository)
    : m_repository(repository),
      m_in_use(nullptr)
{
}

const IDataRepository* CommandsParserState::GetRepository() const
{
    return m_repository;
}

void CommandsParserState::AddBlock(std::unique_ptr<FastFileBlock> block) const
{
    m_repository->Add(std::move(block));
}

void CommandsParserState::SetGame(std::string gameName) const
{
    m_repository->SetGame(std::move(gameName));
}

StructureInformation* CommandsParserState::GetInUse() const
{
    return m_in_use;
}

void CommandsParserState::SetInUse(StructureInformation* structure)
{
    m_in_use = structure;
}

bool CommandsParserState::GetMembersFromParts(const std::string& typeNameValue, StructureInformation* baseType, std::vector<MemberInformation*>& members)
{
    return false;
}

bool CommandsParserState::GetTypenameAndMembersFromParts(const std::string& typeNameValue, StructureInformation*& structure, std::vector<MemberInformation*>& members)
{
    return false;
}
