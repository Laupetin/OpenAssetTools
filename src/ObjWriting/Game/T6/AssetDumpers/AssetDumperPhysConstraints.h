#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperPhysConstraints final : public AbstractAssetDumper<PhysConstraints>
    {
        static cspField_t phys_constraints_fields[];

    protected:
        bool ShouldDump(XAssetInfo<PhysConstraints>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<PhysConstraints>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<PhysConstraints>* asset, FileAPI::File* out) override;
    };
}
