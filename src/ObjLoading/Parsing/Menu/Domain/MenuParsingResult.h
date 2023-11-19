#pragma once

#include "CommonFunctionDef.h"
#include "CommonMenuDef.h"

#include <memory>
#include <vector>

namespace menu
{
    class ParsingResult
    {
    public:
        std::vector<std::unique_ptr<CommonMenuDef>> m_menus;
        std::vector<std::unique_ptr<CommonFunctionDef>> m_functions;
        std::vector<std::string> m_menus_to_load;
    };
} // namespace menu
