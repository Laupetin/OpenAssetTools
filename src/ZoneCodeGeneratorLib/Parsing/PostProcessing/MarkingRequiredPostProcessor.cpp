#include "MarkingRequiredPostProcessor.h"

#include "Domain/Computations/MemberComputations.h"
#include "Domain/Computations/StructureComputations.h"
#include "Domain/Definition/PointerDeclarationModifier.h"

#include <unordered_set>

namespace
{
    bool CalculateRequiresMarking(std::unordered_set<const void*>& visitedStructures, StructureInformation* info)
    {
        if (visitedStructures.contains(info))
            return info->m_requires_marking;

        visitedStructures.emplace(info);

        if (StructureComputations(info).IsAsset())
        {
            info->m_requires_marking = true;
            return true;
        }

        for (const auto& member : info->m_ordered_members)
        {
            // If there is a condition to this member, and it always evaluates to false: Skip this member
            if (member->m_condition && member->m_condition->IsStatic() && member->m_condition->EvaluateNumeric() == 0)
                continue;

            // Skip if it has a pointer evaluation that always resolves to 0
            auto skip = false;
            for (const auto& modifier : member->m_member->m_type_declaration->m_declaration_modifiers)
            {
                if (modifier->GetType() == DeclarationModifierType::POINTER)
                {
                    const auto* pointer = dynamic_cast<PointerDeclarationModifier*>(modifier.get());
                    const auto* countEvaluation = pointer->GetCountEvaluation();

                    if (countEvaluation->IsStatic() && countEvaluation->EvaluateNumeric() == 0)
                    {
                        skip = true;
                        break;
                    }
                }
            }
            if (skip)
                continue;

            // Any script strings, asset refs and assets need to be processed.
            if (member->m_is_script_string || !member->m_asset_ref.empty() || member->m_type && StructureComputations(member->m_type).IsAsset())
            {
                info->m_requires_marking = true;
                return true;
            }

            if (member->m_type != nullptr && member->m_type != info && CalculateRequiresMarking(visitedStructures, member->m_type))
            {
                info->m_requires_marking = true;
                return true;
            }
        }

        info->m_requires_marking = false;
        return false;
    }
} // namespace

bool MarkingRequiredPostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allInfos = repository->GetAllStructureInformation();

    std::unordered_set<const void*> visitedStructures;
    for (const auto& info : allInfos)
    {
        CalculateRequiresMarking(visitedStructures, info);
    }

    return true;
}
