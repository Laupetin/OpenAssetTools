#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace tracer
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetTracer>
    {
    public:
        explicit DumperT6(const AssetPool<T6::AssetTracer::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetTracer::Type>& asset) override;
    };
} // namespace tracer
