#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace raw_file
{
    class DumperIW6 final : public AbstractAssetDumper<IW6::AssetRawFile>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetRawFile::Type>& asset) override;
    };
} // namespace raw_file
