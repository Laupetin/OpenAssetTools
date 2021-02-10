#pragma once

#include "IEvaluation.h"
#include "Domain/Definition/EnumMember.h"

class OperandStatic final : public IEvaluation
{
public:
    const int m_value;
    EnumMember* const m_enum_member;

    explicit OperandStatic(int value);
    OperandStatic(int value, EnumMember* enumMember);

    _NODISCARD EvaluationType GetType() const override;
    _NODISCARD bool IsStatic() const override;
    _NODISCARD int EvaluateNumeric() const override;
};
