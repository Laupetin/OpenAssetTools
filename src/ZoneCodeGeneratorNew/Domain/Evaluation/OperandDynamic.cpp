#include "OperandDynamic.h"

#include <cassert>

OperandDynamic::OperandDynamic(StructureInformation* structure)
    : m_structure(structure)
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
