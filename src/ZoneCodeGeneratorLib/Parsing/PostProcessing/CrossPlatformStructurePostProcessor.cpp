#include "CrossPlatformStructurePostProcessor.h"

#include "Domain/Computations/StructureComputations.h"
#include "Domain/Definition/PointerDeclarationModifier.h"

#include <algorithm>
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

    void CalculateCrossPlatformStructureGraph(std::unordered_set<const void*>& visitedGraph,
                                              std::unordered_set<const void*>& visitedCalculations,
                                              StructureInformation* info)
    {
        if (!visitedGraph.emplace(info).second)
            return;

        for (const auto& member : info->m_ordered_members)
        {
            if (member->m_type != nullptr && member->m_type != info && !StructureComputations(member->m_type).IsAsset())
                CalculateCrossPlatformStructureGraph(visitedGraph, visitedCalculations, member->m_type);
        }

        CalculateHasMatchingCrossPlatformStructure(visitedCalculations, info);
    }
} // namespace

bool CrossPlatformStructurePostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allInfos = repository->GetAllStructureInformation();

    if (repository->GetWordSize() == OWN_WORD_SIZE)
    {
        for (const auto& info : allInfos)
            info->m_has_matching_cross_platform_structure = true;

        std::unordered_set<const void*> visitedGraph;
        std::unordered_set<const void*> visitedCalculations;
        for (const auto& info : allInfos)
        {
            if (info->m_word_size != WordSize::UNKNOWN && info->m_word_size != OWN_WORD_SIZE)
                CalculateCrossPlatformStructureGraph(visitedGraph, visitedCalculations, info);
        }
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
