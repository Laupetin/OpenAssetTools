#include "AbstractMenuConverter.h"

#include <format>
#include <iostream>

using namespace menu;

AbstractMenuConverter::AbstractMenuConverter(const bool disableOptimizations, ISearchPath& searchPath, MemoryManager& memory, AssetCreationContext& context)
    : m_disable_optimizations(disableOptimizations),
      m_search_path(searchPath),
      m_memory(memory),
      m_context(context)
{
}

void AbstractMenuConverter::PrintConversionExceptionDetails(const MenuConversionException& e)
{
    std::cerr << "ERROR while converting menu:\n";
    std::cerr << std::format("  Menu: {}\n", e.m_menu->m_name);

    if (e.m_item)
    {
        std::cerr << std::format("Item: {}\n", e.m_item->m_name);
    }

    std::cerr << std::format("  Message: {}\n", e.m_message);
}

const char* AbstractMenuConverter::ConvertString(const std::string& str) const
{
    if (str.empty())
        return nullptr;

    return m_memory.Dup(str.c_str());
}
