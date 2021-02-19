#pragma once

#include <memory>
#include <string>

#include "Utils/ClassUtils.h"
#include "Persistence/IDataRepository.h"

class CommandsParserState
{
    IDataRepository* m_repository;
    StructureInformation* m_in_use;

public:
    explicit CommandsParserState(IDataRepository* repository);

    _NODISCARD const IDataRepository* GetRepository() const;

    void AddBlock(std::unique_ptr<FastFileBlock> block) const;
    void SetGame(std::string gameName) const;

    _NODISCARD StructureInformation* GetInUse() const;
    void SetInUse(StructureInformation* structure);

    bool GetMembersFromParts(const std::string& typeNameValue, StructureInformation* baseType, std::vector<MemberInformation*>& members);
    bool GetTypenameAndMembersFromParts(const std::string& typeNameValue, StructureInformation*& structure, std::vector<MemberInformation*>& members);
};
