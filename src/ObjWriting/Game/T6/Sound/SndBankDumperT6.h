#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace sound
{
    class SndBankDumperT6 final : public AbstractAssetDumper<T6::AssetSoundBank>
    {
    public:
        explicit SndBankDumperT6(const AssetPool<T6::AssetSoundBank::Type>& pool);
        void Dump(AssetDumpingContext& context) override;

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<Asset<9, T6::SndBank>::Type>& asset) override;
    };
} // namespace sound
