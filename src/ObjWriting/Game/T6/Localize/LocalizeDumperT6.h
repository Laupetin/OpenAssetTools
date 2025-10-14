#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace localize
{
    class DumperT6 final : public IAssetDumper<T6::LocalizeEntry>
    {
    public:
        [[nodiscard]] size_t GetProgressTotalCount(const AssetPool<T6::LocalizeEntry>& pool) const override;
        void DumpPool(AssetDumpingContext& context, const AssetPool<T6::LocalizeEntry>& pool) override;
    };
} // namespace localize
