#pragma once

#include <ostream>

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Model/XModel/XModelExportWriter.h"
#include "Utils/DistinctMapper.h"

namespace IW4
{
    class AssetDumperXModel final : public AbstractAssetDumper<XModel>
    {
        static void DumpObjLod(AssetDumpingContext& context, XAssetInfo<XModel>* asset, unsigned lod);
        static void DumpObjMatMaterial(AssetDumpingContext& context, const Material* material, std::ostream& stream);
        static void DumpObjMat(AssetDumpingContext& context, XAssetInfo<XModel>* asset);
        static void DumpObj(AssetDumpingContext& context, XAssetInfo<XModel>* asset);

        static void AddBonesToWriter(const AssetDumpingContext& context, AbstractXModelWriter& writer, const XModel* model);
        static void AddMaterialsToWriter(AbstractXModelWriter& writer, DistinctMapper<Material*>& materialMapper, const XModel* model);
        static void AddObjectsToWriter(AbstractXModelWriter& writer, const XModelSurfs* modelSurfs);
        static void AddVerticesToWriter(AbstractXModelWriter& writer, const XModelSurfs* modelSurfs);
        static void AllocateBoneWeights(const XModelSurfs* modelSurfs, XModelVertexBoneWeightCollection& weightCollection);
        static void AddVertexBoneWeights(AbstractXModelWriter& writer, const XModelSurfs* modelSurfs, XModelVertexBoneWeightCollection& weightCollection);
        static void AddFacesToWriter(AbstractXModelWriter& writer, const DistinctMapper<Material*>& materialMapper, const XModelSurfs* modelSurfs, int baseSurfaceIndex);
        static void DumpXModelExportLod(const AssetDumpingContext& context, XAssetInfo<XModel>* asset, const unsigned lod);
        static void DumpXModelExport(const AssetDumpingContext& context, XAssetInfo<XModel>* asset);

    protected:
        bool ShouldDump(XAssetInfo<XModel>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset) override;
    };
}
