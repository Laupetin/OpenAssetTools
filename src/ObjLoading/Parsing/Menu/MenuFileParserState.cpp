#include "MenuFileParserState.h"

using namespace menu;

MenuFileParserState::EventHandlerConditionState::EventHandlerConditionState(std::unique_ptr<CommonEventHandlerCondition> condition)
    : m_in_condition_elements(true),
      m_condition(std::move(condition))
{
}

MenuFileParserState::MenuFileParserState(const FeatureLevel featureLevel)
    : m_feature_level(featureLevel),
      m_in_global_scope(false)
{
}
