#pragma once

#include "Domain/Definition/Variable.h"
#include "Domain/Evaluation/IEvaluation.h"
#include "Domain/FastFile/FastFileBlock.h"
#include "StructureInformation.h"

#include <memory>

class StructureInformation;

class MemberInformation
{
public:
    MemberInformation(StructureInformation* parent, StructureInformation* type, Variable* member);

    StructureInformation* m_parent;
    StructureInformation* m_type;
    Variable* m_member;
    bool m_is_string;
    bool m_is_script_string;
    bool m_is_reusable;
    bool m_is_leaf;
    std::unique_ptr<IEvaluation> m_condition;
    std::unique_ptr<IEvaluation> m_alloc_alignment;
    std::unique_ptr<CustomAction> m_post_load_action;
    const FastFileBlock* m_fast_file_block;
    std::string m_asset_ref;
};
