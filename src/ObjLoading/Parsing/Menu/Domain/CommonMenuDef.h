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
        bool m_fullscreen;

        std::vector<std::unique_ptr<CommonItemDef>> m_items;
    };
}
