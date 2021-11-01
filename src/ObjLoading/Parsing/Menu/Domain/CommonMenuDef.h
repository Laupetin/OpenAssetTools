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
        std::vector<std::unique_ptr<CommonItemDef>> m_items;
    };
}
