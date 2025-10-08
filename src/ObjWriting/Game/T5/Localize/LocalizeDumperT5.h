#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace localize
{
    class DumperT5 final : public IAssetDumper<T5::LocalizeEntry>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<T5::LocalizeEntry>* pool) override;
    };
} // namespace localize
