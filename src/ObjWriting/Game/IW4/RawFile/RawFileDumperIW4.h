#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4::raw_file
{
    class Dumper final : public AbstractAssetDumper<RawFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset) override;
    };
} // namespace IW4::raw_file
