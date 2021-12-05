#pragma once

#include "Utils/ClassUtils.h"

class SimpleExpressionValue;
class ISimpleExpression
{
protected:
    ISimpleExpression() = default;
public:
    virtual ~ISimpleExpression() = default;
    ISimpleExpression(const ISimpleExpression& other) = default;
    ISimpleExpression(ISimpleExpression&& other) noexcept = default;
    ISimpleExpression& operator=(const ISimpleExpression& other) = default;
    ISimpleExpression& operator=(ISimpleExpression&& other) noexcept = default;

    _NODISCARD virtual bool IsStatic() const = 0;
    _NODISCARD virtual SimpleExpressionValue Evaluate() const = 0;
};

// Include SimpleExpressionValue after definition to avoid "base class not defined"
#include "SimpleExpressionValue.h"