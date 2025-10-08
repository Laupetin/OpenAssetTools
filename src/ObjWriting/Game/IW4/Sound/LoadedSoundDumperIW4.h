#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace sound
{
    class LoadedSoundDumperIW4 final : public AbstractAssetDumper<IW4::LoadedSound>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::LoadedSound>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::LoadedSound>* asset) override;
    };
} // namespace sound
