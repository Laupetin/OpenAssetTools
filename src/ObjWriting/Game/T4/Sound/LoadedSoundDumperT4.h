#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T4/T4.h"

namespace sound
{
    class LoadedSoundDumperT4 final : public AbstractAssetDumper<T4::AssetLoadedSound>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T4::AssetLoadedSound::Type>& asset) override;
    };
} // namespace sound
