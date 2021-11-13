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

MenuFileParserState::MenuFileParserState(const FeatureLevel featureLevel, const MenuAssetZoneState* zoneState)
    : MenuFileParserState(featureLevel)
{
    for(const auto& function : zoneState->m_functions)
    {
        m_functions_by_name.emplace(std::make_pair(function->m_name, function.get()));
    }

    for(const auto& menu : zoneState->m_menus)
    {
        m_menus_by_name.emplace(std::make_pair(menu->m_name, menu.get()));
    }
}
