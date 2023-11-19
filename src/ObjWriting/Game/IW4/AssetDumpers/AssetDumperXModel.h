#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Model/Obj/ObjWriter.h"
#include "Model/XModel/AbstractXModelWriter.h"
#include "Utils/DistinctMapper.h"

namespace IW4
{
    class AssetDumperXModel final : public AbstractAssetDumper<XModel>
    {
        static GfxImage* GetMaterialColorMap(const Material* material);
        static GfxImage* GetMaterialNormalMap(const Material* material);
        static GfxImage* GetMaterialSpecularMap(const Material* material);

        static void AddObjMaterials(ObjWriter& writer, DistinctMapper<Material*>& materialMapper, const XModel* model);
        static void AddObjObjects(ObjWriter& writer, const DistinctMapper<Material*>& materialMapper, const XModelSurfs* modelSurfs, int baseSurfaceIndex);
        static void AddObjVertices(ObjWriter& writer, const XModelSurfs* modelSurfs);
        static void AddObjFaces(ObjWriter& writer, const XModelSurfs* modelSurfs);
        static void DumpObjLod(const AssetDumpingContext& context, XAssetInfo<XModel>* asset, const unsigned lod);
        static void DumpObjMat(const AssetDumpingContext& context, XAssetInfo<XModel>* asset);
        static void DumpObj(AssetDumpingContext& context, XAssetInfo<XModel>* asset);

        static void AddXModelBones(const AssetDumpingContext& context, AbstractXModelWriter& writer, const XModel* model);
        static void AddXModelMaterials(AbstractXModelWriter& writer, DistinctMapper<Material*>& materialMapper, const XModel* model);
        static void AddXModelObjects(AbstractXModelWriter& writer, const XModelSurfs* modelSurfs);
        static void AddXModelVertices(AbstractXModelWriter& writer, const XModelSurfs* modelSurfs);
        static void AllocateXModelBoneWeights(const XModelSurfs* modelSurfs, XModelVertexBoneWeightCollection& weightCollection);
        static void AddXModelVertexBoneWeights(AbstractXModelWriter& writer, const XModelSurfs* modelSurfs, XModelVertexBoneWeightCollection& weightCollection);
        static void
            AddXModelFaces(AbstractXModelWriter& writer, const DistinctMapper<Material*>& materialMapper, const XModelSurfs* modelSurfs, int baseSurfaceIndex);
        static void DumpXModelExportLod(const AssetDumpingContext& context, XAssetInfo<XModel>* asset, unsigned lod);
        static void DumpXModelExport(AssetDumpingContext& context, XAssetInfo<XModel>* asset);

    protected:
        bool ShouldDump(XAssetInfo<XModel>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset) override;
    };
} // namespace IW4
