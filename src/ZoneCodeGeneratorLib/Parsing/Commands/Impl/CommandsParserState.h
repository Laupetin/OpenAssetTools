#pragma once

#include "Persistence/IDataRepository.h"

#include <memory>
#include <string>

class CommandsParserState
{
public:
    explicit CommandsParserState(IDataRepository* repository);

    [[nodiscard]] const IDataRepository* GetRepository() const;

    void AddBlock(std::unique_ptr<FastFileBlock> block) const;
    void SetArchitecture(Architecture architecture) const;
    void SetGame(std::string gameName) const;

    [[nodiscard]] StructureInformation* GetInUse() const;
    void SetInUse(StructureInformation* structure);

    bool GetMembersFromTypename(const std::string& typeNameValue, const StructureInformation* baseType, std::vector<MemberInformation*>& members) const;
    bool GetTypenameAndMembersFromTypename(const std::string& typeNameValue, StructureInformation*& structure, std::vector<MemberInformation*>& members) const;

private:
    IDataRepository* m_repository;
    StructureInformation* m_in_use;
};
