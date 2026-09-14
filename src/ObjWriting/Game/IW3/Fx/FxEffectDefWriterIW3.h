#pragma once

#include "Game/IW3/IW3.h"

#include <memory>
#include <ostream>

namespace fx
{
    class IEffectDefWriterIW3
    {
    public:
        virtual ~IEffectDefWriterIW3() = default;

        virtual void Write(const IW3::FxEffectDef& effect) = 0;
    };

    std::unique_ptr<IEffectDefWriterIW3> CreateEffectDefWriterIW3(std::ostream& stream);
} // namespace fx
