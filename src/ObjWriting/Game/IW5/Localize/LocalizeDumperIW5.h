#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5::localize
{
    class Dumper final : public IAssetDumper<LocalizeEntry>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<LocalizeEntry>* pool) override;
    };
} // namespace IW5::localize
