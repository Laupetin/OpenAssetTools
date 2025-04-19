#include "MemberLeafsPostProcessor.h"

#include "Domain/Computations/MemberComputations.h"
#include "Domain/Definition/PointerDeclarationModifier.h"

namespace
{
    bool MemberIsLeaf(MemberInformation* member)
    {
        if (member->m_is_string || member->m_is_script_string)
            return false;

        if (member->m_type != nullptr && !member->m_type->m_is_leaf)
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

        const MemberComputations computations(member);

        if (computations.HasDynamicArraySize())
            return false;

        return true;
    }

    void ProcessInfo(StructureInformation* info)
    {
        for (const auto& member : info->m_ordered_members)
        {
            member->m_is_leaf = MemberIsLeaf(member.get());
        }
    }
} // namespace

bool MemberLeafsPostProcessor::PostProcess(IDataRepository* repository)
{
    const auto& allInfos = repository->GetAllStructureInformation();

    for (const auto& info : allInfos)
    {
        ProcessInfo(info);
    }

    return true;
}
