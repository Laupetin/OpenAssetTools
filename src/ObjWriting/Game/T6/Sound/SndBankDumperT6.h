#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::sound
{
    class SndBankDumper final : public IAssetDumper<SndBank>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<SndBank>* pool) override;
    };
} // namespace T6::sound
