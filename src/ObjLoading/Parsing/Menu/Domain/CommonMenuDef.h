#pragma once

#include <string>
#include <vector>

#include "CommonItemDef.h"

namespace menu
{
    class CommonMenuDef
    {
    public:
        std::string m_name;
        bool m_full_screen;
        bool m_screen_space;
        bool m_decoration;

        std::vector<std::unique_ptr<CommonItemDef>> m_items;
    };
}
