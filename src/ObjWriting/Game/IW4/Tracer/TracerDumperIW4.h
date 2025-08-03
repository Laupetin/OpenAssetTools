#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace IW4::tracer
{
    class Dumper final : public AbstractAssetDumper<TracerDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<TracerDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset) override;
    };
} // namespace IW4::tracer
