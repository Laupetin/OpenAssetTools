#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace sound
{
    class LoadedSoundDumperIW3 final : public AbstractAssetDumper<IW3::AssetLoadedSound>
    {
    public:
        explicit LoadedSoundDumperIW3(const AssetPool<IW3::AssetLoadedSound::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetLoadedSound::Type>& asset) override;
    };
} // namespace sound
