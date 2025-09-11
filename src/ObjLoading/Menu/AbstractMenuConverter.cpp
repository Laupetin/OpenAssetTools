#include "AbstractMenuConverter.h"

#include "Utils/Logging/Log.h"

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
    con::error("ERROR while converting menu:");
    con::error("  Menu: {}", e.m_menu->m_name);

    if (e.m_item)
    {
        con::error("Item: {}", e.m_item->m_name);
    }

    con::error("  Message: {}", e.m_message);
}

const char* AbstractMenuConverter::ConvertString(const std::string& str) const
{
    if (str.empty())
        return nullptr;

    return m_memory.Dup(str.c_str());
}
