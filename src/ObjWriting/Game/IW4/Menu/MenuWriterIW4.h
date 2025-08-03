#pragma once

#include "Game/IW4/IW4.h"
#include "Menu/IMenuWriter.h"

#include <memory>
#include <string>

namespace IW4::menu
{
    class IWriterIW4 : public ::menu::IWriter
    {
    public:
        virtual void WriteFunctionDef(const std::string& functionName, const Statement_s* statement) = 0;
        virtual void WriteMenu(const menuDef_t& menu) = 0;
    };

    std::unique_ptr<IWriterIW4> CreateMenuWriter(std::ostream& stream);
} // namespace IW4::menu
