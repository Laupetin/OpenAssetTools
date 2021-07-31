#pragma once

#include <ostream>

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperXModel final : public AbstractAssetDumper<XModel>
    {
        static void DumpObjLod(AssetDumpingContext& context, XAssetInfo<XModel>* asset, unsigned lod);
        static void DumpObjMatMaterial(AssetDumpingContext& context, const Material* material, std::ostream& stream);
        static void DumpObjMat(AssetDumpingContext& context, XAssetInfo<XModel>* asset);
        static void DumpObj(AssetDumpingContext& context, XAssetInfo<XModel>* asset);
        static void DumpXModelExportLod(AssetDumpingContext& context, XAssetInfo<XModel>* asset, unsigned lod);
        static void DumpXModelExport(AssetDumpingContext& context, XAssetInfo<XModel>* asset);

    protected:
        bool ShouldDump(XAssetInfo<XModel>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset) override;
    };
}
