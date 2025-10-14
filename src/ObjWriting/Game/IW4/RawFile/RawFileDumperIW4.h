#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace raw_file
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::RawFile>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::RawFile>& asset) override;
    };
} // namespace raw_file
