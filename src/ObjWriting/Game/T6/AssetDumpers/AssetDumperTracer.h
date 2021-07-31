#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6
{
    class AssetDumperTracer final : public AbstractAssetDumper<TracerDef>
    {
        static InfoString CreateInfoString(XAssetInfo<TracerDef>* asset);

    protected:
        bool ShouldDump(XAssetInfo<TracerDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset) override;
    };
}
