#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace localize
{
    class DumperIW4 final : public IAssetDumper<IW4::LocalizeEntry>
    {
    public:
        [[nodiscard]] size_t GetProgressTotalCount(const AssetPool<IW4::LocalizeEntry>& pool) const override;
        void DumpPool(AssetDumpingContext& context, const AssetPool<IW4::LocalizeEntry>& pool) override;
    };
} // namespace localize
