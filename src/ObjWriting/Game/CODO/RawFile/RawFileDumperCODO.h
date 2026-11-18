#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/CODO/CODO.h"

namespace raw_file
{
    class DumperCODO final : public AbstractAssetDumper<CODO::AssetRawFile>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<CODO::AssetRawFile::Type>& asset) override;
    };
} // namespace raw_file
