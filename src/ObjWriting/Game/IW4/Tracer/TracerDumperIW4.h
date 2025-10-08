#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace tracer
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::TracerDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::TracerDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::TracerDef>* asset) override;
    };
} // namespace tracer
