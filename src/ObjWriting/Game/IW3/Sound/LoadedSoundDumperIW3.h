#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace IW3::sound
{
    class LoadedSoundDumper final : public AbstractAssetDumper<LoadedSound>
    {
    protected:
        bool ShouldDump(XAssetInfo<LoadedSound>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LoadedSound>* asset) override;
    };
} // namespace IW3::sound
