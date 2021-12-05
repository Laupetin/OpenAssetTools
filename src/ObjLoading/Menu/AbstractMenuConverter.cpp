#include "AbstractMenuConverter.h"

#include <iostream>

using namespace menu;

AbstractMenuConverter::AbstractMenuConverter(const bool legacyMode, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
    : m_legacy_mode(legacyMode),
      m_search_path(searchPath),
      m_memory(memory),
      m_manager(manager)
{
}

void AbstractMenuConverter::PrintConversionExceptionDetails(const MenuConversionException& e)
{
    std::cout << "ERROR while converting menu:\n";
    std::cout << "  Menu: " << e.m_menu->m_name << "\n";

    if (e.m_item)
    {
        std::cout << "Item: ";

        if (!e.m_item->m_name.empty())
        {
            std::cout << e.m_item->m_name << "\n";
        }

        std::cout << "\n";
    }

    std::cout << "  Message: " << e.m_message << "\n";
}

const char* AbstractMenuConverter::ConvertString(const std::string& str) const
{
    if (str.empty())
        return nullptr;

    return m_memory->Dup(str.c_str());
}
