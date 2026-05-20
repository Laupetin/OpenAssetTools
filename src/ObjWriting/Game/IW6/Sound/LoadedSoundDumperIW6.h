#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace sound
{
    class LoadedSoundDumperIW6 final : public AbstractAssetDumper<IW6::AssetLoadedSound>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetLoadedSound::Type>& asset) override;
    };
} // namespace sound
