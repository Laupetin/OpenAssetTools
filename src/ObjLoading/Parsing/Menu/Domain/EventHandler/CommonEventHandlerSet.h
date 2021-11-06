#pragma once

#include <memory>
#include <vector>

#include "ICommonEventHandlerElement.h"

namespace menu
{
    class CommonEventHandlerSet
    {
    public:
        std::vector<std::unique_ptr<ICommonEventHandlerElement>> m_elements;

        CommonEventHandlerSet();
        explicit CommonEventHandlerSet(std::vector<std::unique_ptr<ICommonEventHandlerElement>> elements);
    };
}
