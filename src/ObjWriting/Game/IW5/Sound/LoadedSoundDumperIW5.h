#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace sound
{
    class LoadedSoundDumperIW5 final : public AbstractAssetDumper<IW5::AssetLoadedSound>
    {
    public:
        explicit LoadedSoundDumperIW5(const AssetPool<IW5::AssetLoadedSound::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetLoadedSound::Type>& asset) override;
    };
} // namespace sound
