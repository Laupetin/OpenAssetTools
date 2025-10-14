#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace sound
{
    class SndDriverGlobalsDumperT6 final : public IAssetDumper<T6::SndDriverGlobals>
    {
    public:
        [[nodiscard]] size_t GetProgressTotalCount(const AssetPool<T6::SndDriverGlobals>& pool) const override;
        void DumpPool(AssetDumpingContext& context, const AssetPool<T6::SndDriverGlobals>& pool) override;
    };
} // namespace sound
