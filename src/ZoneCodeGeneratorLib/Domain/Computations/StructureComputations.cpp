#include "StructureComputations.h"

#include <cassert>

#include "MemberComputations.h"

StructureComputations::StructureComputations(const StructureInformation* structure)
    : m_info(structure)
{
    assert(m_info != nullptr);
}

bool StructureComputations::IsAsset() const
{
    return m_info->m_asset_enum_entry != nullptr;
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

std::vector<MemberInformation*> StructureComputations::GetUsedMembers() const
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
            if (!member->m_is_leaf && !MemberComputations(member.get()).ShouldIgnore())
                members.push_back(member.get());
        }
    }

    return members;
}