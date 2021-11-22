#pragma once

#include <memory>

#include "CommonEventHandlerSet.h"
#include "ICommonEventHandlerElement.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

namespace menu
{
    class CommonEventHandlerCondition final : public ICommonEventHandlerElement
    {
    public:
        std::unique_ptr<ISimpleExpression> m_condition;
        std::unique_ptr<CommonEventHandlerSet> m_condition_elements;
        std::unique_ptr<CommonEventHandlerSet> m_else_elements;

        CommonEventHandlerCondition();
        CommonEventHandlerCondition(std::unique_ptr<ISimpleExpression> condition, std::unique_ptr<CommonEventHandlerSet> conditionElements,
                                    std::unique_ptr<CommonEventHandlerSet> elseElements);

        _NODISCARD CommonEventHandlerElementType GetType() const override;
    };
}
