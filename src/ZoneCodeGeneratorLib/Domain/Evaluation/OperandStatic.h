#pragma once

#include "Domain/Definition/EnumMember.h"
#include "IEvaluation.h"

class OperandStatic final : public IEvaluation
{
public:
    const int m_value;
    EnumMember* const m_enum_member;

    explicit OperandStatic(int value);
    explicit OperandStatic(EnumMember* enumMember);

    _NODISCARD EvaluationType GetType() const override;
    _NODISCARD bool IsStatic() const override;
    _NODISCARD int EvaluateNumeric() const override;
};
