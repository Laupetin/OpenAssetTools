#pragma once

#include "Game/IW5/IW5.h"
#include "Menu/IMenuWriter.h"

#include <memory>
#include <string>

namespace menu
{
    class IWriterIW5 : public IWriter
    {
    public:
        virtual void WriteFunctionDef(const std::string& functionName, const IW5::Statement_s* statement) = 0;
        virtual void WriteMenu(const IW5::menuDef_t& menu) = 0;
    };

    std::unique_ptr<IWriterIW5> CreateMenuWriterIW5(std::ostream& stream);
} // namespace menu
