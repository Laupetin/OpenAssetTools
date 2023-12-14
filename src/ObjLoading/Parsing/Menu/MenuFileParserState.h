#pragma once

#include "Domain/CommonFunctionDef.h"
#include "Domain/CommonMenuDef.h"
#include "Domain/EventHandler/CommonEventHandlerCondition.h"
#include "Domain/EventHandler/CommonEventHandlerSet.h"
#include "Domain/MenuFeatureLevel.h"
#include "MenuAssetZoneState.h"

#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include <vector>

namespace menu
{
    class MenuFileParserState
    {
    public:
        class EventHandlerConditionState
        {
        public:
            bool m_in_condition_elements;
            bool m_auto_skip;
            CommonEventHandlerCondition* m_condition;

            explicit EventHandlerConditionState(CommonEventHandlerCondition* condition);
            EventHandlerConditionState(CommonEventHandlerCondition* condition, bool autoSkip);
        };

        const FeatureLevel m_feature_level;
        const bool m_permissive_mode;

        std::vector<std::string> m_menus_to_load;
        std::vector<std::unique_ptr<CommonFunctionDef>> m_functions;
        std::vector<std::unique_ptr<CommonMenuDef>> m_menus;

        // Function names are case-insensitive, therefore keys are always lowercase
        std::map<std::string, CommonFunctionDef*> m_functions_by_name;

        std::map<std::string, CommonMenuDef*> m_menus_by_name;

        bool m_in_global_scope;
        CommonFunctionDef* m_current_function;
        CommonMenuDef* m_current_menu;
        CommonItemDef* m_current_item;
        CommonEventHandlerSet* m_current_event_handler_set;

        std::ostringstream m_current_script;
        bool m_current_script_statement_terminated;
        std::stack<EventHandlerConditionState> m_condition_stack;
        CommonEventHandlerSet* m_current_nested_event_handler_set;

        explicit MenuFileParserState(FeatureLevel featureLevel, bool permissiveMode);
        MenuFileParserState(FeatureLevel featureLevel, bool permissiveMode, const MenuAssetZoneState* zoneState);
    };
} // namespace menu
