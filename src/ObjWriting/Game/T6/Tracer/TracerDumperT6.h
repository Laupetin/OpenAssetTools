#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace tracer
{
    class DumperT6 final : public AbstractAssetDumper<T6::TracerDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::TracerDef>& asset) override;
    };
} // namespace tracer
