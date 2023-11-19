#pragma once
#include "Parsing/Menu/Domain/CommonMenuDef.h"

namespace menu
{
    class MenuConversionException final : public std::exception
    {
    public:
        const CommonMenuDef* m_menu;
        const CommonItemDef* m_item;
        std::string m_message;

        explicit MenuConversionException(std::string message, const CommonMenuDef* menu);
        MenuConversionException(std::string message, const CommonMenuDef* menu, const CommonItemDef* item);
    };
} // namespace menu
