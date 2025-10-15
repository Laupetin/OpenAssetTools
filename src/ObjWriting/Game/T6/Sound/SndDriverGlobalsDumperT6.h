#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace sound
{
    class SndDriverGlobalsDumperT6 final : public AbstractSingleProgressAssetDumper<T6::AssetSoundDriverGlobals>
    {
    public:
        explicit SndDriverGlobalsDumperT6(const AssetPool<T6::AssetSoundDriverGlobals::Type>& pool);

        void Dump(AssetDumpingContext& context) override;
    };
} // namespace sound
