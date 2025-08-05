#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace raw_file
{
    class DumperT5 final : public AbstractAssetDumper<T5::RawFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<T5::RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T5::RawFile>* asset) override;
    };
} // namespace raw_file
