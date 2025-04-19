#pragma once

#include "IEvaluation.h"
#include "OperationType.h"
#include "Utils/ClassUtils.h"

#include <memory>

class Operation final : public IEvaluation
{
public:
    explicit Operation(const OperationType* type);
    Operation(const OperationType* type, std::unique_ptr<IEvaluation> operand1, std::unique_ptr<IEvaluation> operand2);

    [[nodiscard]] EvaluationType GetType() const override;
    [[nodiscard]] bool IsStatic() const override;
    [[nodiscard]] int EvaluateNumeric() const override;

    [[nodiscard]] bool Operand1NeedsParenthesis() const;
    [[nodiscard]] bool Operand2NeedsParenthesis() const;

    const OperationType* const m_operation_type;
    std::unique_ptr<IEvaluation> m_operand1;
    std::unique_ptr<IEvaluation> m_operand2;
};
