#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace localize
{
    class DumperIW5 final : public IAssetDumper<IW5::LocalizeEntry>
    {
    public:
        [[nodiscard]] size_t GetProgressTotalCount(const AssetPool<IW5::LocalizeEntry>& pool) const override;
        void DumpPool(AssetDumpingContext& context, const AssetPool<IW5::LocalizeEntry>& pool) override;
    };
} // namespace localize
