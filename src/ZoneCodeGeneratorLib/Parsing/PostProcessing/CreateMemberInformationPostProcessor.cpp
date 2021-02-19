#include "CreateMemberInformationPostProcessor.h"

#include <algorithm>

bool CreateMemberInformationPostProcessor::CreateMemberInformationForStructure(IDataRepository* repository, StructureInformation* structure) const
{
    for(const auto& member : structure->m_definition->m_members)
    {
        StructureInformation* typeInfo = nullptr;
        const auto* memberDefinition = dynamic_cast<const DefinitionWithMembers*>(member->m_type_declaration->m_type);

        if(memberDefinition != nullptr)
            typeInfo = repository->GetInformationFor(memberDefinition);

        structure->m_ordered_members.emplace_back(std::make_unique<MemberInformation>(structure, typeInfo, member.get()));
    }

    return true;
}

bool CreateMemberInformationPostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allStructureInformation = repository->GetAllStructureInformation();

    return std::all_of(allStructureInformation.begin(), allStructureInformation.end(), [this, repository](StructureInformation* structure)
    {
        return CreateMemberInformationForStructure(repository, structure);
    });
}
