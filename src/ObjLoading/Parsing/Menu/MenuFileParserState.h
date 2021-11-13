#pragma once

#include <map>
#include <memory>
#include <vector>
#include <sstream>
#include <stack>

#include "MenuAssetZoneState.h"
#include "Domain/CommonFunctionDef.h"
#include "Domain/CommonMenuDef.h"
#include "Domain/MenuFeatureLevel.h"
#include "Domain/EventHandler/CommonEventHandlerSet.h"
#include "Domain/EventHandler/CommonEventHandlerCondition.h"

namespace menu
{
    class MenuFileParserState
    {
    public:
        class EventHandlerConditionState
        {
        public:
            bool m_in_condition_elements;
            CommonEventHandlerCondition* m_condition;

            explicit EventHandlerConditionState(CommonEventHandlerCondition* condition);
        };

        const FeatureLevel m_feature_level;

        std::vector<std::string> m_menus_to_load;
        std::vector<std::unique_ptr<CommonFunctionDef>> m_functions;
        std::vector<std::unique_ptr<CommonMenuDef>> m_menus;

        std::map<std::string, CommonFunctionDef*> m_functions_by_name;
        std::map<std::string, CommonMenuDef*> m_menus_by_name;

        bool m_in_global_scope;
        CommonFunctionDef* m_current_function;
        CommonMenuDef* m_current_menu;
        CommonItemDef* m_current_item;
        CommonEventHandlerSet* m_current_event_handler_set;
        
        std::ostringstream m_current_script;
        std::stack<EventHandlerConditionState> m_condition_stack;
        CommonEventHandlerSet* m_current_nested_event_handler_set;

        explicit MenuFileParserState(FeatureLevel featureLevel);
        MenuFileParserState(FeatureLevel featureLevel, const MenuAssetZoneState* zoneState);
    };
}
