#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperSndBank final : public IAssetDumper<SndBank>
    {
        class Internal;

    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<SndBank>* pool) override;
    };
}
