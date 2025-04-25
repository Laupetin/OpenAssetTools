#include "CommandsParserState.h"

namespace
{
    MemberInformation* GetMemberWithName(const std::string& memberName, const StructureInformation* type)
    {
        for (const auto& member : type->m_ordered_members)
        {
            if (member->m_member->m_name == memberName)
            {
                return member.get();
            }
        }

        return nullptr;
    }

    bool GetNextTypenameSeparatorPos(const std::string& typeNameValue, const size_t startPos, size_t& separatorPos)
    {
        const auto typeNameValueSize = typeNameValue.size();
        for (auto currentHead = startPos + 1; currentHead < typeNameValueSize; currentHead++)
        {
            if (typeNameValue[currentHead] == ':' && typeNameValue[currentHead - 1] == ':')
            {
                separatorPos = currentHead - 1;
                return true;
            }
        }

        return false;
    }

    bool ExtractMembersFromTypenameInternal(const std::string& typeNameValue,
                                            size_t typeNameOffset,
                                            const StructureInformation* type,
                                            std::vector<MemberInformation*>& members)
    {
        auto startOffset = typeNameOffset;
        while (GetNextTypenameSeparatorPos(typeNameValue, typeNameOffset, typeNameOffset))
        {
            auto* foundMember = GetMemberWithName(std::string(typeNameValue, startOffset, typeNameOffset - startOffset), type);

            if (foundMember == nullptr)
                return false;

            members.push_back(foundMember);
            type = foundMember->m_type;
            typeNameOffset += 2;
            startOffset = typeNameOffset;
        }

        auto* foundMember = GetMemberWithName(std::string(typeNameValue, startOffset, typeNameValue.size() - startOffset), type);
        if (foundMember == nullptr)
            return false;

        members.push_back(foundMember);
        return true;
    }
} // namespace

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

void CommandsParserState::SetArchitecture(const Architecture architecture) const
{
    m_repository->SetArchitecture(architecture);
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

bool CommandsParserState::GetMembersFromTypename(const std::string& typeNameValue,
                                                 const StructureInformation* baseType,
                                                 std::vector<MemberInformation*>& members) const
{
    return m_in_use != nullptr && ExtractMembersFromTypenameInternal(typeNameValue, 0, m_in_use, members)
           || ExtractMembersFromTypenameInternal(typeNameValue, 0, baseType, members);
}

bool CommandsParserState::GetTypenameAndMembersFromTypename(const std::string& typeNameValue,
                                                            StructureInformation*& structure,
                                                            std::vector<MemberInformation*>& members) const
{
    if (m_in_use != nullptr)
    {
        if (ExtractMembersFromTypenameInternal(typeNameValue, 0, m_in_use, members))
        {
            structure = m_in_use;
            return true;
        }
        members.clear();
    }

    DataDefinition* foundDefinition = nullptr;
    auto currentSeparatorPos = 0uz;
    while (GetNextTypenameSeparatorPos(typeNameValue, currentSeparatorPos, currentSeparatorPos))
    {
        std::string currentTypename(typeNameValue, 0, currentSeparatorPos);
        currentSeparatorPos += 2;

        foundDefinition = m_repository->GetDataDefinitionByName(currentTypename);
        if (foundDefinition != nullptr)
            break;
    }

    if (foundDefinition == nullptr)
    {
        currentSeparatorPos = typeNameValue.size();
        foundDefinition = m_repository->GetDataDefinitionByName(typeNameValue);
    }

    if (foundDefinition == nullptr)
        return false;

    const auto* definitionWithMembers = dynamic_cast<DefinitionWithMembers*>(foundDefinition);
    if (definitionWithMembers == nullptr)
        return false;

    structure = m_repository->GetInformationFor(definitionWithMembers);
    if (currentSeparatorPos >= typeNameValue.size())
        return true;

    return ExtractMembersFromTypenameInternal(typeNameValue, currentSeparatorPos, structure, members);
}
