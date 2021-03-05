#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperPhysConstraints final : public AbstractFileDumper<PhysConstraints>
    {
        static cspField_t phys_constraints_fields[];

    protected:
        bool ShouldDump(XAssetInfo<PhysConstraints>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<PhysConstraints>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysConstraints>* asset, std::ostream& stream) override;
    };
}
