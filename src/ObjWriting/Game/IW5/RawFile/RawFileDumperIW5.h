#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5::raw_file
{
    class Dumper final : public AbstractAssetDumper<RawFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset) override;
    };
} // namespace IW5::raw_file
