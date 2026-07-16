#pragma once

#include "Game/IW3/IW3.h"
#include "Menu/IMenuWriter.h"

#include <memory>
#include <ostream>

namespace menu
{
    class IWriterIW3 : public IWriter
    {
    public:
        virtual void WriteMenu(const IW3::menuDef_t& menu) = 0;
    };

    std::unique_ptr<IWriterIW3> CreateMenuWriterIW3(std::ostream& stream);
} // namespace menu
