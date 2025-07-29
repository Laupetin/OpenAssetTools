#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::localize
{
    class Dumper final : public IAssetDumper<LocalizeEntry>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<LocalizeEntry>* pool) override;
    };
} // namespace T6::localize
