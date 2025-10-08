#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace sound
{
    class SndDriverGlobalsDumperT6 final : public IAssetDumper<T6::SndDriverGlobals>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<T6::SndDriverGlobals>* pool) override;
    };
} // namespace sound
