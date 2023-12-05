#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/T6/T6.h"

namespace T6::sound
{
    void DumpSoundData(const AssetDumpingContext& context, const SndBank* sndBank);
}