#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace sound
{
    class LoadedSoundDumperIW5 final : public AbstractAssetDumper<IW5::LoadedSound>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW5::LoadedSound>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW5::LoadedSound>* asset) override;
    };
} // namespace sound
