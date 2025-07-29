#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace T5::localize
{
    class Dumper final : public IAssetDumper<LocalizeEntry>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<LocalizeEntry>* pool) override;
    };
} // namespace T5::localize
