#include "CommonEventHandlerSet.h"

using namespace menu;

CommonEventHandlerSet::CommonEventHandlerSet() = default;

CommonEventHandlerSet::CommonEventHandlerSet(std::vector<std::unique_ptr<ICommonEventHandlerElement>> elements)
    : m_elements(std::move(elements))
{
}
