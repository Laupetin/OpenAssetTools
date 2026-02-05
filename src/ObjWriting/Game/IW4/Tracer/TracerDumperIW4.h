#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace tracer
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetTracer>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetTracer::Type>& asset) override;
    };
} // namespace tracer
