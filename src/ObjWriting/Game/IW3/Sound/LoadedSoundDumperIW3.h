#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace sound
{
    class LoadedSoundDumperIW3 final : public AbstractAssetDumper<IW3::LoadedSound>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::LoadedSound>& asset) override;
    };
} // namespace sound
