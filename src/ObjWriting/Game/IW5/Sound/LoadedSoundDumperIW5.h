#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5::sound
{
    class LoadedSoundDumper final : public AbstractAssetDumper<LoadedSound>
    {
    protected:
        bool ShouldDump(XAssetInfo<LoadedSound>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LoadedSound>* asset) override;
    };
} // namespace IW5::sound
