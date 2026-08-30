#pragma once

#include "Game/T4/T4.h"
#include "Menu/IMenuWriter.h"

#include <memory>
#include <ostream>

namespace menu
{
    class IWriterT4 : public IWriter
    {
    public:
        virtual void WriteMenu(const T4::menuDef_t& menu) = 0;
    };

    std::unique_ptr<IWriterT4> CreateMenuWriterT4(std::ostream& stream);
} // namespace menu
