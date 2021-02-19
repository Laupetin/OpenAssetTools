#include "OperandDynamic.h"

#include <cassert>

OperandDynamic::OperandDynamic(StructureInformation* structure)
    : m_structure(structure)
{
}

OperandDynamic::OperandDynamic(StructureInformation* structure, std::vector<MemberInformation*> referencedMemberChain, std::vector<std::unique_ptr<IEvaluation>> arrayIndices)
    : m_structure(structure),
      m_referenced_member_chain(std::move(referencedMemberChain)),
      m_array_indices(std::move(arrayIndices))
{
}

EvaluationType OperandDynamic::GetType() const
{
    return EvaluationType::OPERAND_DYNAMIC;
}

bool OperandDynamic::IsStatic() const
{
    return false;
}

int OperandDynamic::EvaluateNumeric() const
{
    assert(false);
    return 0;
}
