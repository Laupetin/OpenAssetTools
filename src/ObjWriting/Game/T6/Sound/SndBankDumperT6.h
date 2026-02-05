#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace sound
{
    class SndBankDumperT6 final : public AbstractAssetDumper<T6::AssetSoundBank>
    {
    public:
        void Dump(AssetDumpingContext& context) override;

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetSoundBank::Type>& asset) override;
    };
} // namespace sound
