#pragma once

#include "Persistence/IDataRepository.h"
#include "Utils/ClassUtils.h"

#include <memory>
#include <string>

class CommandsParserState
{
    IDataRepository* m_repository;
    StructureInformation* m_in_use;

    static MemberInformation* GetMemberWithName(const std::string& memberName, StructureInformation* type);
    static bool GetNextTypenameSeparatorPos(const std::string& typeNameValue, unsigned startPos, unsigned& separatorPos);
    static bool ExtractMembersFromTypenameInternal(const std::string& typeNameValue,
                                                   unsigned typeNameOffset,
                                                   StructureInformation* type,
                                                   std::vector<MemberInformation*>& members);

public:
    explicit CommandsParserState(IDataRepository* repository);

    _NODISCARD const IDataRepository* GetRepository() const;

    void AddBlock(std::unique_ptr<FastFileBlock> block) const;
    void SetArchitecture(Architecture architecture) const;
    void SetGame(std::string gameName) const;

    _NODISCARD StructureInformation* GetInUse() const;
    void SetInUse(StructureInformation* structure);

    bool GetMembersFromTypename(const std::string& typeNameValue, StructureInformation* baseType, std::vector<MemberInformation*>& members) const;
    bool GetTypenameAndMembersFromTypename(const std::string& typeNameValue, StructureInformation*& structure, std::vector<MemberInformation*>& members) const;
};
