#include "MenuFileParserState.h"

using namespace menu;

MenuFileParserState::EventHandlerConditionState::EventHandlerConditionState(CommonEventHandlerCondition* condition)
    : m_in_condition_elements(true),
      m_condition(condition)
{
}

MenuFileParserState::MenuFileParserState(const FeatureLevel featureLevel)
    : m_feature_level(featureLevel),
      m_in_global_scope(false),
      m_current_function(nullptr),
      m_current_menu(nullptr),
      m_current_item(nullptr),
      m_current_event_handler_set(nullptr),
      m_current_nested_event_handler_set(nullptr)
{
}
