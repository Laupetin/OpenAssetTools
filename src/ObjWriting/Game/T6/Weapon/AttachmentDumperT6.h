#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace attachment
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetAttachment>
    {
    public:
        explicit DumperT6(const AssetPool<T6::AssetAttachment::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetAttachment::Type>& asset) override;
    };
} // namespace attachment
