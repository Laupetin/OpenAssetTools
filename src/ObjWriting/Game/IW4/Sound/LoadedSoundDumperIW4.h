#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4::sound
{
    class LoadedSoundDumper final : public AbstractAssetDumper<LoadedSound>
    {
    protected:
        bool ShouldDump(XAssetInfo<LoadedSound>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LoadedSound>* asset) override;
    };
} // namespace IW4::sound
