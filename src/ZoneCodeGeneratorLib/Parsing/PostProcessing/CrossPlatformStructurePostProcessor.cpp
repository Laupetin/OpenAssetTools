#include "CrossPlatformStructurePostProcessor.h"

#include "Domain/Definition/PointerDeclarationModifier.h"

#include <unordered_set>

namespace
{
    bool CalculateHasMatchingCrossPlatformStructure(std::unordered_set<const void*>& visitedStructures, StructureInformation* info)
    {
        if (visitedStructures.find(info) != visitedStructures.end())
            return info->m_has_matching_cross_platform_structure;

        visitedStructures.emplace(info);

        for (const auto& member : info->m_ordered_members)
        {
            for (const auto& modifier : member->m_member->m_type_declaration->m_declaration_modifiers)
            {
                if (modifier->GetType() == DeclarationModifierType::POINTER)
                {
                    info->m_has_matching_cross_platform_structure = false;
                    return false;
                }
            }

            if (member->m_type != nullptr && member->m_type != info && !CalculateHasMatchingCrossPlatformStructure(visitedStructures, member->m_type))
            {
                info->m_has_matching_cross_platform_structure = false;
                return false;
            }
        }

        info->m_has_matching_cross_platform_structure = true;
        return true;
    }
} // namespace

bool CrossPlatformStructurePostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allInfos = repository->GetAllStructureInformation();

    if (repository->GetArchitecture() == OWN_ARCHITECTURE)
    {
        for (const auto& info : allInfos)
            info->m_has_matching_cross_platform_structure = true;
    }
    else
    {
        std::unordered_set<const void*> visitedStructures;
        for (const auto& info : allInfos)
        {
            CalculateHasMatchingCrossPlatformStructure(visitedStructures, info);
        }
    }

    return true;
}
