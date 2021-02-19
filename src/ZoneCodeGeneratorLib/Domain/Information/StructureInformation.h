#pragma once

#include "Utils/ClassUtils.h"
#include "MemberInformation.h"
#include "Domain/Definition/DefinitionWithMembers.h"
#include "Domain/Definition/EnumMember.h"
#include "Domain/Extension/CustomAction.h"
#include "Domain/FastFile/FastFileBlock.h"

class MemberInformation;
class StructureInformation
{
public:
    bool m_has_non_default_fast_file_align;
    unsigned m_custom_fast_file_align;

    DefinitionWithMembers* const m_definition;
    EnumMember* m_asset_enum_entry;

    std::vector<StructureInformation*> m_usages;
    std::vector<std::unique_ptr<MemberInformation>> m_ordered_members;

    bool m_is_leaf;

    bool m_non_embedded_reference_exists;
    bool m_single_pointer_reference_exists;
    bool m_array_pointer_reference_exists;
    bool m_array_reference_exists;
    bool m_reference_from_non_default_normal_block_exists;

    std::unique_ptr<CustomAction> m_post_load_action;
    const FastFileBlock* m_block;
    std::vector<MemberInformation*> m_name_chain;

    explicit StructureInformation(DefinitionWithMembers* definition);
};
