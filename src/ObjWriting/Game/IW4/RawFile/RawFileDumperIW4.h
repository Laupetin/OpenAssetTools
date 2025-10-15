#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace raw_file
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetRawFile>
    {
    public:
        explicit DumperIW4(const AssetPool<IW4::AssetRawFile::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetRawFile::Type>& asset) override;
    };
} // namespace raw_file
