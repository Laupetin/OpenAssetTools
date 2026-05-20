#pragma once

#include "Game/IW6/IW6.h"
#include "Menu/IMenuWriter.h"

#include <memory>
#include <string>

namespace menu
{
    class IWriterIW6 : public IWriter
    {
    public:
        virtual void WriteFunctionDef(const std::string& functionName, const IW6::Statement_s* statement) = 0;
        virtual void WriteMenu(const IW6::menuDef_t& menu) = 0;
    };

    std::unique_ptr<IWriterIW6> CreateMenuWriterIW6(std::ostream& stream);
} // namespace menu
