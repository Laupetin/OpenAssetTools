#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperLocalizeEntry final : public IAssetDumper<LocalizeEntry>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<LocalizeEntry>* pool) override;
    };
}
