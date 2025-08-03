#pragma once

#include "Game/IW5/IW5.h"
#include "Menu/IMenuWriter.h"

#include <memory>
#include <string>

namespace IW5::menu
{
    class IWriterIW5 : public ::menu::IWriter
    {
    public:
        virtual void WriteFunctionDef(const std::string& functionName, const Statement_s* statement) = 0;
        virtual void WriteMenu(const menuDef_t& menu) = 0;
    };

    std::unique_ptr<IWriterIW5> CreateMenuWriter(std::ostream& stream);
} // namespace IW5::menu
