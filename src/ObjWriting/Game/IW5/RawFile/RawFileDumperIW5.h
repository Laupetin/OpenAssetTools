#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace raw_file
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::RawFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW5::RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW5::RawFile>* asset) override;
    };
} // namespace raw_file
