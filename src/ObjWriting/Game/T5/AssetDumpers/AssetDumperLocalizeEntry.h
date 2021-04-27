#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace T5
{
    class AssetDumperLocalizeEntry final : public IAssetDumper<LocalizeEntry>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<LocalizeEntry>* pool) override;
    };
}
