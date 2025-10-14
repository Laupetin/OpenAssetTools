#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace localize
{
    class DumperIW3 final : public IAssetDumper<IW3::LocalizeEntry>
    {
    public:
        [[nodiscard]] size_t GetProgressTotalCount(const AssetPool<IW3::LocalizeEntry>& pool) const override;
        void DumpPool(AssetDumpingContext& context, const AssetPool<IW3::LocalizeEntry>& pool) override;
    };
} // namespace localize
