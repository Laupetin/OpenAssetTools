#include "OperandStatic.h"

OperandStatic::OperandStatic(const int value)
    : m_value(value),
      m_enum_member(nullptr)
{
}

OperandStatic::OperandStatic(const int value, EnumMember* enumMember)
    : m_value(value),
      m_enum_member(enumMember)
{
}

EvaluationType OperandStatic::GetType()
{
    return EvaluationType::OPERAND_STATIC;
}

bool OperandStatic::IsStatic()
{
    return true;
}

int OperandStatic::EvaluateNumeric()
{
    return m_value;
}
