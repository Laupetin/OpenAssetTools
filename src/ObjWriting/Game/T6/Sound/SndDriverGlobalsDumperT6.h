#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace sound
{
    class SndDriverGlobalsDumperT6 final : public AbstractAssetDumper<T6::AssetSoundDriverGlobals>
    {
    public:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetSoundDriverGlobals::Type>& asset) override;
    };
} // namespace sound
