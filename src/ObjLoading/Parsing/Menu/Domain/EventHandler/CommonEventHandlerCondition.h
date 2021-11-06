#pragma once

#include <memory>

#include "CommonEventHandlerSet.h"
#include "ICommonEventHandlerElement.h"
#include "Parsing/Menu/Domain/Expression/ICommonExpression.h"

namespace menu
{
    class CommonEventHandlerCondition final : public ICommonEventHandlerElement
    {
    public:
        std::unique_ptr<ICommonExpression> m_condition;
        std::unique_ptr<CommonEventHandlerSet> m_condition_elements;
        std::unique_ptr<CommonEventHandlerSet> m_else_elements;

        CommonEventHandlerCondition();
        explicit CommonEventHandlerCondition(std::unique_ptr<ICommonExpression> condition, std::unique_ptr<CommonEventHandlerSet> conditionElements,
                                             std::unique_ptr<CommonEventHandlerSet> elseElements);

        CommonEventHandlerElementType GetType() override;
    };
}
