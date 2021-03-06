#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "Utils/InfoString.h"

namespace T6
{
    class AssetDumperPhysConstraints final : public AbstractAssetDumper<PhysConstraints>
    {
        static constexpr const char* FILE_TYPE_STR = "PHYSCONSTRAINTS";
        static constexpr const char* GDF_NAME = "physconstraints.gdf";
        static cspField_t phys_constraints_fields[];

        static InfoString CreateInfoString(XAssetInfo<PhysConstraints>* asset);

    protected:
        bool ShouldDump(XAssetInfo<PhysConstraints>* asset) override;
        bool CanDumpAsRaw() override;
        bool CanDumpAsGdtEntry() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<PhysConstraints>* asset) override;
        GdtEntry DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<PhysConstraints>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<PhysConstraints>* asset, std::ostream& stream) override;
    };
}
