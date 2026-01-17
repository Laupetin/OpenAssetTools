#include "StructureComputations.h"

#include "MemberComputations.h"

#include <cassert>

StructureComputations::StructureComputations(const StructureInformation* structure)
    : m_info(structure)
{
    assert(m_info != nullptr);
}

bool StructureComputations::IsAsset() const
{
    return !m_info->m_asset_name.empty();
}

MemberInformation* StructureComputations::GetDynamicMember() const
{
    for (const auto& member : m_info->m_ordered_members)
    {
        if (MemberComputations(member.get()).IsDynamicMember())
            return member.get();
    }

    return nullptr;
}

bool StructureComputations::HasNonDynamicMember() const
{
    for (const auto& member : m_info->m_ordered_members)
    {
        const MemberComputations memberComputations(member.get());
        if (!memberComputations.ShouldIgnore() && !memberComputations.IsAfterPartialLoad())
            return true;
    }

    return false;
}

std::vector<MemberInformation*> StructureComputations::GetUsedMembers(const bool includeLeafs) const
{
    std::vector<MemberInformation*> members;

    if (m_info->m_definition->GetType() == DataDefinitionType::UNION && GetDynamicMember() != nullptr)
    {
        for (const auto& member : m_info->m_ordered_members)
        {
            if (!MemberComputations(member.get()).ShouldIgnore())
                members.push_back(member.get());
        }
    }
    else
    {
        for (const auto& member : m_info->m_ordered_members)
        {
            if ((includeLeafs || !member->m_is_leaf) && !MemberComputations(member.get()).ShouldIgnore())
                members.push_back(member.get());
        }
    }

    return members;
}

bool StructureComputations::IsInTempBlock() const
{
    return m_info->m_block != nullptr && m_info->m_block->m_type == FastFileBlockType::TEMP;
}
