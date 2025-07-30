#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace T5::raw_file
{
    class Dumper final : public AbstractAssetDumper<RawFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset) override;
    };
} // namespace T5::raw_file
