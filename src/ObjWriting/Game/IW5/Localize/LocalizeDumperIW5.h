#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace localize
{
    class DumperIW5 final : public IAssetDumper<IW5::LocalizeEntry>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<IW5::LocalizeEntry>* pool) override;
    };
} // namespace localize
