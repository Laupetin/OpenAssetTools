#include "MenuConversionException.h"

using namespace menu;

MenuConversionException::MenuConversionException(std::string message, const CommonMenuDef* menu)
    : m_menu(menu),
      m_item(nullptr),
      m_message(std::move(message))
{
}

MenuConversionException::MenuConversionException(std::string message, const CommonMenuDef* menu, const CommonItemDef* item)
    : m_menu(menu),
      m_item(item),
      m_message(std::move(message))
{
}
