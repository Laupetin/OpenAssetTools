#pragma once

#include "Game/IW4/IW4.h"
#include "Menu/IMenuWriter.h"

#include <memory>
#include <string>

namespace menu
{
    class IWriterIW4 : public IWriter
    {
    public:
        virtual void WriteFunctionDef(const std::string& functionName, const IW4::Statement_s* statement) = 0;
        virtual void WriteMenu(const IW4::menuDef_t& menu) = 0;
    };

    std::unique_ptr<IWriterIW4> CreateMenuWriterIW4(std::ostream& stream);
} // namespace menu
