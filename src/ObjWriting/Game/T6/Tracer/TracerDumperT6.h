#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6::tracer
{
    class Dumper final : public AbstractAssetDumper<TracerDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<TracerDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset) override;
    };
} // namespace T6::tracer
