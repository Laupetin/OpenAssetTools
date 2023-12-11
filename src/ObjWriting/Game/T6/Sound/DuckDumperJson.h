#pragma once

#include "Game/T6/T6.h"

namespace T6::sound
{
    class DuckDumperJson
    {
        virtual void DumpSndBankDucks(const SndBank* sndBank) const = 0;
    };
} // namespace T6::sound
