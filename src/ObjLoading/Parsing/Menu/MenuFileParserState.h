#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Domain/CommonFunctionDef.h"
#include "Domain/CommonMenuDef.h"
#include "Domain/MenuFeatureLevel.h"

class MenuFileParserState
{
public:
    const MenuFeatureLevel m_feature_level;

    std::vector<std::string> m_menus_to_load;
    std::vector<std::unique_ptr<CommonFunctionDef>> m_functions;
    std::vector<std::unique_ptr<CommonMenuDef>> m_menus;

    std::map<std::string, CommonFunctionDef*> m_functions_by_name;
    std::map<std::string, CommonMenuDef*> m_menus_by_name;

    bool m_in_global_scope;
    std::unique_ptr<CommonFunctionDef> m_current_function;
    std::unique_ptr<CommonMenuDef> m_current_menu;
    std::unique_ptr<CommonItemDef> m_current_item;

    explicit MenuFileParserState(MenuFeatureLevel featureLevel);
};
