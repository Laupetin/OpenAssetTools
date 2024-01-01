#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperSndDriverGlobals final : public IAssetDumper<SndDriverGlobals>
    {
        class Internal;

    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<SndDriverGlobals>* pool) override;
    };
} // namespace T6
