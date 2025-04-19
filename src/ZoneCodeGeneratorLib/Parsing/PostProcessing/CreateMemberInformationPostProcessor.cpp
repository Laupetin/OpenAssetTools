#include "CreateMemberInformationPostProcessor.h"

#include <algorithm>

namespace
{
    bool CreateMemberInformationForStructure(IDataRepository* repository, StructureInformation* structure)
    {
        for (const auto& member : structure->m_definition->m_members)
        {
            StructureInformation* typeInfo = nullptr;
            const auto* currentDefinition = member->m_type_declaration->m_type;

            while (currentDefinition->GetType() == DataDefinitionType::TYPEDEF)
            {
                currentDefinition = dynamic_cast<const TypedefDefinition*>(currentDefinition)->m_type_declaration->m_type;
            }

            const auto* memberDefinition = dynamic_cast<const DefinitionWithMembers*>(currentDefinition);

            if (memberDefinition != nullptr)
                typeInfo = repository->GetInformationFor(memberDefinition);

            structure->m_ordered_members.emplace_back(std::make_unique<MemberInformation>(structure, typeInfo, member.get()));
        }

        return true;
    }
} // namespace

bool CreateMemberInformationPostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allStructureInformation = repository->GetAllStructureInformation();

    return std::ranges::all_of(allStructureInformation,
                               [this, repository](StructureInformation* structure)
                               {
                                   return CreateMemberInformationForStructure(repository, structure);
                               });
}
