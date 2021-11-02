#pragma once

namespace menu
{
    class CommonExpressionValue;
    class ICommonExpression
    {
    protected:
        ICommonExpression() = default;
    public:
        virtual ~ICommonExpression() = default;
        ICommonExpression(const ICommonExpression& other) = default;
        ICommonExpression(ICommonExpression&& other) noexcept = default;
        ICommonExpression& operator=(const ICommonExpression& other) = default;
        ICommonExpression& operator=(ICommonExpression&& other) noexcept = default;

        virtual bool IsStatic() = 0;
        virtual CommonExpressionValue Evaluate() = 0;
    };
}

// Include CommonExpressionValue after definition to avoid "base class not defined"
#include "CommonExpressionValue.h"