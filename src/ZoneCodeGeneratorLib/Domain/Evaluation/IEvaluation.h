#pragma once

enum class EvaluationType
{
    OPERAND_DYNAMIC,
    OPERAND_STATIC,
    OPERATION
};

class IEvaluation
{
public:
    IEvaluation() = default;
    virtual ~IEvaluation() = default;

    IEvaluation(const IEvaluation& other) = default;
    IEvaluation(IEvaluation&& other) noexcept = default;
    IEvaluation& operator=(const IEvaluation& other) = default;
    IEvaluation& operator=(IEvaluation&& other) noexcept = default;

    [[nodiscard]] virtual EvaluationType GetType() const = 0;
    [[nodiscard]] virtual bool IsStatic() const = 0;
    [[nodiscard]] virtual int EvaluateNumeric() const = 0;
};
