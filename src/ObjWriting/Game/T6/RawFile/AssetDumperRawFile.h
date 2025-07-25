#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperRawFile final : public AbstractAssetDumper<RawFile>
    {
        constexpr static size_t ANIMTREE_MAX_SIZE = 0xC000000;

        void DumpAnimtree(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream);

    protected:
        bool ShouldDump(XAssetInfo<RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset) override;
    };
} // namespace T6
