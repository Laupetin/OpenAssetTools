#pragma once

#include <memory>

#include "Utils/ClassUtils.h"
#include "StructureInformation.h"
#include "Domain/Definition/Variable.h"
#include "Domain/FastFile/FastFileBlock.h"
#include "Domain/Evaluation/IEvaluation.h"

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
    const FastFileBlock* m_fast_file_block;

    MemberInformation(StructureInformation* parent, StructureInformation* type, Variable* member);
};
