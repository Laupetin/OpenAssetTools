#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace raw_file
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetRawFile>
    {
    public:
        explicit DumperIW3(const AssetPool<IW3::AssetRawFile::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::RawFile>& asset) override;
    };
} // namespace raw_file
