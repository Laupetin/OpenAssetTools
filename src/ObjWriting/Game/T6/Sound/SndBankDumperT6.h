#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace sound
{
    class SndBankDumperT6 final : public IAssetDumper<T6::SndBank>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<T6::SndBank>* pool) override;
    };
} // namespace sound
