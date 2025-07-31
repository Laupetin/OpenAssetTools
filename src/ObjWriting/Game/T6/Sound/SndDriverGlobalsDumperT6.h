#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::sound
{
    class SndDriverGlobalsDumper final : public IAssetDumper<SndDriverGlobals>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<SndDriverGlobals>* pool) override;
    };
} // namespace T6::sound
