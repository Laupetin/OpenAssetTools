#include "OperandDynamic.h"

#include <cassert>

OperandDynamic::OperandDynamic(StructureInformation* structure)
    : m_structure(structure)
{
}

EvaluationType OperandDynamic::GetType()
{
    return EvaluationType::OPERAND_DYNAMIC;
}

bool OperandDynamic::IsStatic()
{
    return false;
}

int OperandDynamic::EvaluateNumeric()
{
    assert(false);
    return 0;
}
