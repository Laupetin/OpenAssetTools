#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <sstream>
#include <stack>

#include "Domain/CommonFunctionDef.h"
#include "Domain/CommonMenuDef.h"
#include "Domain/MenuFeatureLevel.h"
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
            std::unique_ptr<CommonEventHandlerCondition> m_condition;

            explicit EventHandlerConditionState(std::unique_ptr<CommonEventHandlerCondition> condition);
        };

        const FeatureLevel m_feature_level;

        std::vector<std::string> m_menus_to_load;
        std::vector<std::unique_ptr<CommonFunctionDef>> m_functions;
        std::vector<std::unique_ptr<CommonMenuDef>> m_menus;

        std::map<std::string, CommonFunctionDef*> m_functions_by_name;
        std::map<std::string, CommonMenuDef*> m_menus_by_name;

        bool m_in_global_scope;
        std::unique_ptr<CommonFunctionDef> m_current_function;
        std::unique_ptr<CommonMenuDef> m_current_menu;
        std::unique_ptr<CommonItemDef> m_current_item;
        std::unique_ptr<CommonEventHandlerSet> m_current_event_handler_set;

        std::function<void(MenuFileParserState* state, std::unique_ptr<CommonEventHandlerSet> value)> m_event_handler_set_callback;
        std::ostringstream m_current_script;
        std::stack<EventHandlerConditionState> m_current_condition;

        explicit MenuFileParserState(FeatureLevel featureLevel);
    };
}
