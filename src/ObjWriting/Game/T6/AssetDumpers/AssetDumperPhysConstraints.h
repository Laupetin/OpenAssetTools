#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6
{
    class AssetDumperPhysConstraints final : public AbstractAssetDumper<PhysConstraints>
    {
        static InfoString CreateInfoString(XAssetInfo<PhysConstraints>* asset);

    protected:
        bool ShouldDump(XAssetInfo<PhysConstraints>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysConstraints>* asset) override;
    };
}
