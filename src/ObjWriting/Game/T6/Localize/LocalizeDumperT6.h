#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace localize
{
    class DumperT6 final : public IAssetDumper<T6::LocalizeEntry>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<T6::LocalizeEntry>* pool) override;
    };
} // namespace localize
