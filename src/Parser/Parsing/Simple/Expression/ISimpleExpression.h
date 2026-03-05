#pragma once

#include <string>

class SimpleExpressionValue;

class ISimpleExpressionScopeValues
{
protected:
    ISimpleExpressionScopeValues() = default;

public:
    virtual ~ISimpleExpressionScopeValues() = default;
    ISimpleExpressionScopeValues(const ISimpleExpressionScopeValues& other) = default;
    ISimpleExpressionScopeValues(ISimpleExpressionScopeValues&& other) noexcept = default;
    ISimpleExpressionScopeValues& operator=(const ISimpleExpressionScopeValues& other) = default;
    ISimpleExpressionScopeValues& operator=(ISimpleExpressionScopeValues&& other) noexcept = default;

    [[nodiscard]] virtual SimpleExpressionValue ValueByName(const std::string& name) const = 0;
};

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

    [[nodiscard]] virtual bool Equals(const ISimpleExpression* other) const = 0;
    [[nodiscard]] virtual bool IsStatic() const = 0;
    [[nodiscard]] virtual SimpleExpressionValue EvaluateStatic() const = 0;
    [[nodiscard]] virtual SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const = 0;
};

// Include SimpleExpressionValue after definition to avoid "base class not defined"
#include "SimpleExpressionValue.h"
