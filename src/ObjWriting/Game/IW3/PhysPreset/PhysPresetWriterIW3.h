#pragma once

#include "Game/IW3/IW3.h"
#include "Font/IFontWriter.h"

#include <memory>
#include <string>

namespace font
{
    class IWriterIW3 : public IWriter
    {
    public:
        virtual void WriteFont(const IW3::Font_s& font) = 0;
    };

    std::unique_ptr<IWriterIW3> CreateFontWriterIW3(std::ostream& stream);
} // namespace font
