#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"
#include "Pool/AssetPool.h"

namespace localize
{
    class DumperIW3 final : public AbstractSingleProgressAssetDumper<IW3::AssetLocalize>
    {
    public:
        explicit DumperIW3(const AssetPool<IW3::AssetLocalize::Type>& pool);

        [[nodiscard]] size_t GetProgressTotalCount() const override;
        void Dump(AssetDumpingContext& context) override;
    };
} // namespace localize
