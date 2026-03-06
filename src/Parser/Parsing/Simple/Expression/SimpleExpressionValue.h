#pragma once

#include "ISimpleExpression.h"

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

    [[nodiscard]] bool Equals(const ISimpleExpression* other) const override;
    [[nodiscard]] bool IsStatic() const override;
    [[nodiscard]] SimpleExpressionValue EvaluateStatic() const override;
    [[nodiscard]] SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const override;
    [[nodiscard]] bool IsTruthy() const;
};
