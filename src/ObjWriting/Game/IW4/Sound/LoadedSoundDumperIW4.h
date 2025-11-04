#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace sound
{
    class LoadedSoundDumperIW4 final : public AbstractAssetDumper<IW4::AssetLoadedSound>
    {
    public:
        explicit LoadedSoundDumperIW4(const AssetPool<IW4::AssetLoadedSound::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetLoadedSound::Type>& asset) override;
    };
} // namespace sound
