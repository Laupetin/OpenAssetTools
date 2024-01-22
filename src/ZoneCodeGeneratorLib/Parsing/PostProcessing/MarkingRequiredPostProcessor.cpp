#include "MarkingRequiredPostProcessor.h"

#include "Domain/Computations/MemberComputations.h"
#include "Domain/Computations/StructureComputations.h"
#include "Domain/Definition/PointerDeclarationModifier.h"

#include <unordered_set>

bool MarkingRequiredPostProcessor::RequiresMarking(std::unordered_set<const void*>& visitedStructures, const StructureInformation* info)
{
    if (visitedStructures.find(info) != visitedStructures.end())
        return info->m_requires_marking;

    visitedStructures.emplace(info);

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

        // Any ScriptStrings or Strings need to be processed.
        if (member->m_is_script_string || member->m_is_string || member->m_type && member->m_type->m_asset_enum_entry)
            return true;

        if (member->m_type != nullptr && member->m_type != info && RequiresMarking(visitedStructures, member->m_type))
            return true;
    }

    return false;
}

bool MarkingRequiredPostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allInfos = repository->GetAllStructureInformation();

    std::unordered_set<const void*> visitedStructures;
    for (const auto& info : allInfos)
    {
        info->m_requires_marking = RequiresMarking(visitedStructures, info);
    }

    return true;
}
