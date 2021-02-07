#pragma once

#include <memory>

#include "Utils/ClassUtils.h"
#include "IEvaluation.h"
#include "OperationType.h"

class Operation final : public IEvaluation
{
public:
    const OperationType* const m_operation_type;
    std::unique_ptr<IEvaluation> m_operand1;
    std::unique_ptr<IEvaluation> m_operand2;

    explicit Operation(const OperationType* type);

    EvaluationType GetType() override;
    bool IsStatic() override;
    int EvaluateNumeric() override;

    _NODISCARD bool Operand1NeedsParenthesis() const;
    _NODISCARD bool Operand2NeedsParenthesis() const;
};
