#include "LeafsPostProcessor.h"

#include "Domain/Computations/MemberComputations.h"
#include "Domain/Computations/StructureComputations.h"
#include "Domain/Definition/PointerDeclarationModifier.h"

bool LeafsPostProcessor::IsLeaf(StructureInformation* info)
{
    for (const auto& member : info->m_ordered_members)
    {
        // If there is a condition to this member and it always evaluates to false: Skip this member
        if (member->m_condition && member->m_condition->IsStatic() && member->m_condition->EvaluateNumeric() == 0)
            continue;

        // Any ScriptStrings or Strings need to be processed.
        if (member->m_is_script_string || member->m_is_string)
            return false;

        // If there are any Pointer members that are not always count 0 it needs to be processed.
        for (const auto& modifier : member->m_member->m_type_declaration->m_declaration_modifiers)
        {
            if (modifier->GetType() == DeclarationModifierType::POINTER)
            {
                auto* pointer = dynamic_cast<PointerDeclarationModifier*>(modifier.get());
                const auto* countEvaluation = pointer->GetCountEvaluation();

                if (!countEvaluation->IsStatic() || countEvaluation->EvaluateNumeric() != 0)
                    return false;
            }
        }

        MemberComputations computations(member.get());

        // If the member has an embedded type with dynamic size
        if (computations.HasDynamicArraySize())
            return false;

        if (member->m_type != nullptr
            && member->m_type != info
            && !IsLeaf(member->m_type))
        {
            return false;
        }
    }

    return true;
}

bool LeafsPostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allInfos = repository->GetAllStructureInformation();

    for (const auto& info : allInfos)
    {
        info->m_is_leaf = IsLeaf(info);
    }

    return true;
}
