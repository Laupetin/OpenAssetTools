#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace IW4
{
    class AssetDumperTracer final : public AbstractAssetDumper<TracerDef>
    {
        static InfoString CreateInfoString(XAssetInfo<TracerDef>* asset);

    protected:
        bool ShouldDump(XAssetInfo<TracerDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset) override;
    };
}
