#pragma once

#include "Domain/Definition/EnumMember.h"
#include "IEvaluation.h"

class OperandStatic final : public IEvaluation
{
public:
    explicit OperandStatic(int value);
    explicit OperandStatic(EnumMember* enumMember);

    [[nodiscard]] EvaluationType GetType() const override;
    [[nodiscard]] bool IsStatic() const override;
    [[nodiscard]] int EvaluateNumeric() const override;

    const int m_value;
    EnumMember* const m_enum_member;
};
