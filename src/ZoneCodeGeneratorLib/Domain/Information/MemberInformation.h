#pragma once

#include "Domain/Definition/Variable.h"
#include "Domain/Evaluation/IEvaluation.h"
#include "Domain/FastFile/FastFileBlock.h"
#include "StructureInformation.h"
#include "Utils/ClassUtils.h"

#include <memory>

class StructureInformation;

class MemberInformation
{
public:
    StructureInformation* m_parent;
    StructureInformation* m_type;
    Variable* m_member;
    bool m_is_string;
    bool m_is_script_string;
    bool m_is_reusable;
    bool m_is_leaf;
    std::unique_ptr<IEvaluation> m_condition;
    std::unique_ptr<IEvaluation> m_alloc_alignment;
    const FastFileBlock* m_fast_file_block;
    const EnumMember* m_asset_ref;

    MemberInformation(StructureInformation* parent, StructureInformation* type, Variable* member);
};
