#pragma once

#include <memory>
#include <vector>

#include "CommonFunctionDef.h"
#include "CommonMenuDef.h"

class MenuParsingResult
{
public:
    std::vector<std::unique_ptr<CommonMenuDef>> m_menus;
    std::vector<std::unique_ptr<CommonFunctionDef>> m_functions;
    std::vector<std::string> m_menus_to_load;
};
