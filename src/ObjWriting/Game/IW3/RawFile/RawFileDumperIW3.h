#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace raw_file
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::RawFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW3::RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW3::RawFile>* asset) override;
    };
} // namespace raw_file
