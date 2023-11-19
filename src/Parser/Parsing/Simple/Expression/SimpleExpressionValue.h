#pragma once

#include "ISimpleExpression.h"
#include "Utils/ClassUtils.h"

#include <memory>
#include <string>

class ISimpleExpression;

class SimpleExpressionValue final : public ISimpleExpression
{
public:
    enum class Type
    {
        STRING,
        DOUBLE,
        INT
    };

    Type m_type;
    std::shared_ptr<std::string> m_string_value;

    union
    {
        double m_double_value;
        int m_int_value;
    };

    explicit SimpleExpressionValue(std::string stringValue);
    explicit SimpleExpressionValue(double doubleValue);
    explicit SimpleExpressionValue(int intValue);

    _NODISCARD bool Equals(const ISimpleExpression* other) const override;
    _NODISCARD bool IsStatic() const override;
    _NODISCARD SimpleExpressionValue EvaluateStatic() const override;
    _NODISCARD SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const override;
    _NODISCARD bool IsTruthy() const;
};
