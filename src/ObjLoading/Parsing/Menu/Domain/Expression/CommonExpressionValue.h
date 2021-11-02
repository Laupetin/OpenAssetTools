#pragma once

#include <memory>
#include <string>

#include "ICommonExpression.h"
#include "Utils/ClassUtils.h"

namespace menu
{
    class ICommonExpression;
    class CommonExpressionValue final : public ICommonExpression
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

        explicit CommonExpressionValue(std::string stringValue);
        explicit CommonExpressionValue(double doubleValue);
        explicit CommonExpressionValue(int intValue);

        bool IsStatic() override;
        CommonExpressionValue Evaluate() override;
        _NODISCARD bool IsTruthy() const;
    };
}
