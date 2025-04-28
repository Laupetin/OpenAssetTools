#pragma once

#include "Domain/Definition/DefinitionWithMembers.h"
#include "Domain/Definition/EnumMember.h"
#include "Domain/Extension/CustomAction.h"
#include "Domain/FastFile/FastFileBlock.h"
#include "MemberInformation.h"

#include <memory>
#include <vector>

class MemberInformation;

class StructureInformation
{
public:
    explicit StructureInformation(DefinitionWithMembers* definition);

    DefinitionWithMembers* m_definition;
    std::string m_asset_name;

    std::vector<StructureInformation*> m_usages;
    std::vector<std::unique_ptr<MemberInformation>> m_ordered_members;

    bool m_is_leaf;
    bool m_requires_marking;
    bool m_has_matching_cross_platform_structure;

    bool m_non_embedded_reference_exists;
    bool m_single_pointer_reference_exists;
    bool m_array_pointer_reference_exists;
    bool m_array_reference_exists;
    bool m_reference_from_non_default_normal_block_exists;
    bool m_reusable_reference_exists;

    std::unique_ptr<CustomAction> m_post_load_action;
    const FastFileBlock* m_block;
    std::vector<MemberInformation*> m_name_chain;
};
