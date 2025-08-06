#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace slug
{
    class DumperT6 final : public AbstractAssetDumper<T6::Slug>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::Slug>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::Slug>* asset) override;
    };
} // namespace slug
