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

EvaluationType OperandStatic::GetType() const
{
    return EvaluationType::OPERAND_STATIC;
}

bool OperandStatic::IsStatic() const
{
    return true;
}

int OperandStatic::EvaluateNumeric() const
{
    return m_value;
}
