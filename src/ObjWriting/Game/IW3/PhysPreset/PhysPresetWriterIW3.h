#pragma once

#include "Game/IW3/IW3.h"
#include "PhysPreset/IPhysPresetWriter.h"

#include <memory>
#include <string>

namespace physpreset
{
    class IWriterIW3 : public IWriter
    {
    public:
        virtual void WritePhysPreset(const IW3::PhysPreset& physPreset) = 0;
    };

    std::unique_ptr<IWriterIW3> CreatePhysPresetWriterIW3(std::ostream& stream);
} // namespace font
