#pragma once

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

    virtual bool IsStatic() = 0;
    virtual SimpleExpressionValue Evaluate() = 0;
};

// Include SimpleExpressionValue after definition to avoid "base class not defined"
#include "SimpleExpressionValue.h"