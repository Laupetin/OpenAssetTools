#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::slug
{
    class Dumper final : public AbstractAssetDumper<Slug>
    {
    protected:
        bool ShouldDump(XAssetInfo<Slug>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<Slug>* asset) override;
    };
} // namespace T6::slug
