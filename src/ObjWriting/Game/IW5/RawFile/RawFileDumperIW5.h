#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace raw_file
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::RawFile>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::RawFile>& asset) override;
    };
} // namespace raw_file
