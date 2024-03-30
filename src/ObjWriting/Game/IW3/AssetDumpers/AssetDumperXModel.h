#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"
#include "Utils/DistinctMapper.h"
#include "XModel/AbstractXModelWriter.h"
#include "XModel/Obj/ObjWriter.h"

namespace IW3
{
    class AssetDumperXModel final : public AbstractAssetDumper<XModel>
    {
        static GfxImage* GetMaterialColorMap(const Material* material);
        static GfxImage* GetMaterialNormalMap(const Material* material);
        static GfxImage* GetMaterialSpecularMap(const Material* material);

        static void AddObjMaterials(ObjWriter& writer, DistinctMapper<Material*>& materialMapper, const XModel* model);
        static void AddObjObjects(ObjWriter& writer, const DistinctMapper<Material*>& materialMapper, const XModel* model, unsigned lod);
        static void AddObjVertices(ObjWriter& writer, const XModel* model, unsigned lod);
        static void AddObjFaces(ObjWriter& writer, const XModel* model, unsigned lod);
        static void DumpObjLod(AssetDumpingContext& context, XAssetInfo<XModel>* asset, unsigned lod);
        static void DumpObjMat(const AssetDumpingContext& context, XAssetInfo<XModel>* asset);
        static void DumpObj(AssetDumpingContext& context, XAssetInfo<XModel>* asset);

        static void AddXModelBones(const AssetDumpingContext& context, AbstractXModelWriter& writer, const XModel* model);
        static void AddXModelMaterials(AbstractXModelWriter& writer, DistinctMapper<Material*>& materialMapper, const XModel* model);
        static void AddXModelObjects(AbstractXModelWriter& writer, const XModel* model, unsigned lod);
        static void AddXModelVertices(AbstractXModelWriter& writer, const XModel* model, unsigned lod);
        static void AllocateXModelBoneWeights(const XModel* model, unsigned lod, XModelVertexBoneWeightCollection& weightCollection);
        static void
            AddXModelVertexBoneWeights(AbstractXModelWriter& writer, const XModel* model, unsigned lod, XModelVertexBoneWeightCollection& weightCollection);
        static void AddXModelFaces(AbstractXModelWriter& writer, const DistinctMapper<Material*>& materialMapper, const XModel* model, unsigned lod);
        static void DumpXModelExportLod(const AssetDumpingContext& context, XAssetInfo<XModel>* asset, unsigned lod);
        static void DumpXModelExport(const AssetDumpingContext& context, XAssetInfo<XModel>* asset);

    protected:
        bool ShouldDump(XAssetInfo<XModel>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset) override;
    };
} // namespace IW3
