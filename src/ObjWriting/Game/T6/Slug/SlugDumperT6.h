#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace slug
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetSlug>
    {
    public:
        explicit DumperT6(const AssetPool<T6::AssetSlug::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetSlug::Type>& asset) override;
    };
} // namespace slug
