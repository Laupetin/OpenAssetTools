#include "MenuFileParserState.h"

#include "Utils/StringUtils.h"

using namespace menu;

MenuFileParserState::EventHandlerConditionState::EventHandlerConditionState(CommonEventHandlerCondition* condition)
    : EventHandlerConditionState(condition, false)
{
}

MenuFileParserState::EventHandlerConditionState::EventHandlerConditionState(CommonEventHandlerCondition* condition, const bool autoSkip)
    : m_in_condition_elements(true),
      m_auto_skip(autoSkip),
      m_condition(condition)
{
}

MenuFileParserState::MenuFileParserState(const FeatureLevel featureLevel, const bool permissiveMode)
    : m_feature_level(featureLevel),
      m_permissive_mode(permissiveMode),
      m_in_global_scope(false),
      m_current_function(nullptr),
      m_current_menu(nullptr),
      m_current_item(nullptr),
      m_current_event_handler_set(nullptr),
      m_current_script_statement_terminated(true),
      m_current_nested_event_handler_set(nullptr)
{
}

MenuFileParserState::MenuFileParserState(const FeatureLevel featureLevel, const bool permissiveMode, const MenuAssetZoneState* zoneState)
    : MenuFileParserState(featureLevel, permissiveMode)
{
    for (const auto& function : zoneState->m_functions)
    {
        std::string lowerCaseName(function->m_name);
        utils::MakeStringLowerCase(lowerCaseName);
        m_functions_by_name.emplace(std::make_pair(lowerCaseName, function.get()));
    }

    for (const auto& menu : zoneState->m_menus)
    {
        m_menus_by_name.emplace(std::make_pair(menu->m_name, menu.get()));
    }
}
