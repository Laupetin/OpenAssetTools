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

    virtual EvaluationType GetType() = 0;
    virtual bool IsStatic() = 0;
    virtual int EvaluateNumeric() = 0;
};