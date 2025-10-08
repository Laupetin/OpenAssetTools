#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace localize
{
    class DumperIW4 final : public IAssetDumper<IW4::LocalizeEntry>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<IW4::LocalizeEntry>* pool) override;
    };
} // namespace localize
