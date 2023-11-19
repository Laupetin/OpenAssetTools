#pragma once

#include "ICommonEventHandlerElement.h"

#include <memory>
#include <vector>

namespace menu
{
    class CommonEventHandlerSet
    {
    public:
        std::vector<std::unique_ptr<ICommonEventHandlerElement>> m_elements;

        CommonEventHandlerSet();
        explicit CommonEventHandlerSet(std::vector<std::unique_ptr<ICommonEventHandlerElement>> elements);
    };
} // namespace menu
