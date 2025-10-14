#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace sound
{
    class SndBankDumperT6 final : public IAssetDumper<T6::SndBank>
    {
    public:
        [[nodiscard]] size_t GetProgressTotalCount(const AssetPool<T6::SndBank>& pool) const override;
        void DumpPool(AssetDumpingContext& context, const AssetPool<T6::SndBank>& pool) override;
    };
} // namespace sound
