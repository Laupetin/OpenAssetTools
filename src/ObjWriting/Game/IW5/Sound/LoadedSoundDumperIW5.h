#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace sound
{
    class LoadedSoundDumperIW5 final : public AbstractAssetDumper<IW5::LoadedSound>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::LoadedSound>& asset) override;
    };
} // namespace sound
