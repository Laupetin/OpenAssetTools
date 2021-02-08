#pragma once

#include "Utils/ClassUtils.h"

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

    _NODISCARD virtual EvaluationType GetType() const = 0;
    _NODISCARD virtual bool IsStatic() const = 0;
    _NODISCARD virtual int EvaluateNumeric() const = 0;
};