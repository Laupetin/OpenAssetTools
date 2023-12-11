#pragma once

#include "Game/T6/T6.h"

namespace T6::sound
{
    class RadverbDumperJson
    {
    public:
        virtual void DumpSndBankRadverbs(const SndBank* sndBank) const = 0;
    };
}