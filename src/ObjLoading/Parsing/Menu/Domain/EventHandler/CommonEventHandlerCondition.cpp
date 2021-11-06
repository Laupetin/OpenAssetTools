#include "CommonEventHandlerCondition.h"

using namespace menu;

CommonEventHandlerCondition::CommonEventHandlerCondition()
= default;

CommonEventHandlerCondition::CommonEventHandlerCondition(std::unique_ptr<ICommonExpression> condition, std::unique_ptr<CommonEventHandlerSet> conditionElements,
                                                         std::unique_ptr<CommonEventHandlerSet> elseElements)
    : m_condition(std::move(condition)),
      m_condition_elements(std::move(conditionElements)),
      m_else_elements(std::move(elseElements))
{
}

CommonEventHandlerElementType CommonEventHandlerCondition::GetType()
{
    return CommonEventHandlerElementType::CONDITION;
}
