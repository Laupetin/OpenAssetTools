#include "AssetDumperXModel.h"

#include "Game/IW3/CommonIW3.h"
#include "Math/Quaternion.h"
#include "Model/XModel/XModelExportWriter.h"
#include "ObjWriting.h"
#include "Utils/HalfFloat.h"
#include "Utils/QuatInt16.h"

#include <cassert>
#include <sstream>

using namespace IW3;

bool AssetDumperXModel::ShouldDump(XAssetInfo<XModel>* asset)
{
    return !asset->m_name.empty() && asset->m_name[0] != ',';
}

GfxImage* AssetDumperXModel::GetMaterialColorMap(const Material* material)
{
    std::vector<MaterialTextureDef*> potentialTextureDefs;

    for (auto textureIndex = 0u; textureIndex < material->textureCount; textureIndex++)
    {
        MaterialTextureDef* def = &material->textureTable[textureIndex];

        if (def->semantic == TS_COLOR_MAP)
            potentialTextureDefs.push_back(def);
    }

    if (potentialTextureDefs.empty())
        return nullptr;
    if (potentialTextureDefs.size() == 1)
        return potentialTextureDefs[0]->u.image;

    for (const auto* def : potentialTextureDefs)
    {
        if (def->nameStart == 'c' && def->nameEnd == 'p')
            return def->u.image;
    }

    return potentialTextureDefs[0]->u.image;
}

GfxImage* AssetDumperXModel::GetMaterialNormalMap(const Material* material)
{
    std::vector<MaterialTextureDef*> potentialTextureDefs;

    for (auto textureIndex = 0u; textureIndex < material->textureCount; textureIndex++)
    {
        MaterialTextureDef* def = &material->textureTable[textureIndex];

        if (def->semantic == TS_NORMAL_MAP)
            potentialTextureDefs.push_back(def);
    }

    if (potentialTextureDefs.empty())
        return nullptr;
    if (potentialTextureDefs.size() == 1)
        return potentialTextureDefs[0]->u.image;

    for (const auto* def : potentialTextureDefs)
    {
        if (def->nameStart == 'n' && def->nameEnd == 'p')
            return def->u.image;
    }

    return potentialTextureDefs[0]->u.image;
}

GfxImage* AssetDumperXModel::GetMaterialSpecularMap(const Material* material)
{
    std::vector<MaterialTextureDef*> potentialTextureDefs;

    for (auto textureIndex = 0u; textureIndex < material->textureCount; textureIndex++)
    {
        MaterialTextureDef* def = &material->textureTable[textureIndex];

        if (def->semantic == TS_SPECULAR_MAP)
            potentialTextureDefs.push_back(def);
    }

    if (potentialTextureDefs.empty())
        return nullptr;
    if (potentialTextureDefs.size() == 1)
        return potentialTextureDefs[0]->u.image;

    for (const auto* def : potentialTextureDefs)
    {
        if (def->nameStart == 's' && def->nameEnd == 'p')
            return def->u.image;
    }

    return potentialTextureDefs[0]->u.image;
}

void AssetDumperXModel::AddObjMaterials(ObjWriter& writer, DistinctMapper<Material*>& materialMapper, const XModel* model)
{
    if (!model->materialHandles)
        return;

    for (auto surfIndex = 0u; surfIndex < model->numsurfs; surfIndex++)
    {
        Material* material = model->materialHandles[surfIndex];
        if (!materialMapper.Add(material))
            continue;

        MtlMaterial mtl;
        mtl.materialName = std::string(material->info.name);

        GfxImage* colorMap = GetMaterialColorMap(material);
        GfxImage* normalMap = GetMaterialNormalMap(material);
        GfxImage* specularMap = GetMaterialSpecularMap(material);

        if (colorMap != nullptr)
            mtl.colorMapName = colorMap->name;
        if (normalMap != nullptr)
            mtl.normalMapName = normalMap->name;
        if (specularMap != nullptr)
            mtl.specularMapName = specularMap->name;

        writer.AddMaterial(std::move(mtl));
    }
}

void AssetDumperXModel::AddObjObjects(ObjWriter& writer, const DistinctMapper<Material*>& materialMapper, const XModel* model, const unsigned lod)
{
    const auto surfCount = model->lodInfo[lod].numsurfs;
    const auto baseSurfIndex = model->lodInfo[lod].surfIndex;

    for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
    {
        ObjObject object;
        object.name = "surf" + std::to_string(surfIndex);
        object.materialIndex = static_cast<int>(materialMapper.GetDistinctPositionByInputPosition(surfIndex + baseSurfIndex));

        writer.AddObject(std::move(object));
    }
}

void AssetDumperXModel::AddObjVertices(ObjWriter& writer, const XModel* model, const unsigned lod)
{
    const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
    const auto surfCount = model->lodInfo[lod].numsurfs;

    for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
    {
        const auto& surface = surfs[surfIndex];

        for (auto vertexIndex = 0u; vertexIndex < surface.vertCount; vertexIndex++)
        {
            const auto& v = surface.verts0[vertexIndex];
            vec2_t uv;
            vec3_t normalVec;

            Common::Vec2UnpackTexCoords(v.texCoord, &uv);
            Common::Vec3UnpackUnitVec(v.normal, &normalVec);

            ObjVertex objVertex{};
            ObjNormal objNormal{};
            ObjUv objUv{};
            objVertex.coordinates[0] = v.xyz[0];
            objVertex.coordinates[1] = v.xyz[2];
            objVertex.coordinates[2] = -v.xyz[1];
            objNormal.normal[0] = normalVec[0];
            objNormal.normal[1] = normalVec[2];
            objNormal.normal[2] = -normalVec[1];
            objUv.uv[0] = uv[0];
            objUv.uv[1] = 1.0f - uv[1];

            writer.AddVertex(static_cast<int>(surfIndex), objVertex);
            writer.AddNormal(static_cast<int>(surfIndex), objNormal);
            writer.AddUv(static_cast<int>(surfIndex), objUv);
        }
    }
}

void AssetDumperXModel::AddObjFaces(ObjWriter& writer, const XModel* model, const unsigned lod)
{
    const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
    const auto surfCount = model->lodInfo[lod].numsurfs;

    for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
    {
        const auto& surface = surfs[surfIndex];
        for (auto triIndex = 0u; triIndex < surface.triCount; triIndex++)
        {
            const auto& tri = surface.triIndices[triIndex];

            ObjFace face{};
            face.vertexIndex[0] = tri[2] + surface.baseVertIndex;
            face.vertexIndex[1] = tri[1] + surface.baseVertIndex;
            face.vertexIndex[2] = tri[0] + surface.baseVertIndex;
            face.normalIndex[0] = face.vertexIndex[0];
            face.normalIndex[1] = face.vertexIndex[1];
            face.normalIndex[2] = face.vertexIndex[2];
            face.uvIndex[0] = face.vertexIndex[0];
            face.uvIndex[1] = face.vertexIndex[1];
            face.uvIndex[2] = face.vertexIndex[2];
            writer.AddFace(static_cast<int>(surfIndex), face);
        }
    }
}

void AssetDumperXModel::DumpObjMat(const AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    const auto* model = asset->Asset();
    const auto matFile = context.OpenAssetFile("model_export/" + std::string(model->name) + ".mtl");

    if (!matFile)
        return;

    ObjWriter writer(context.m_zone->m_game->GetShortName(), context.m_zone->m_name);
    DistinctMapper<Material*> materialMapper(model->numsurfs);

    AddObjMaterials(writer, materialMapper, model);
    writer.WriteMtl(*matFile);
}

void AssetDumperXModel::DumpObjLod(AssetDumpingContext& context, XAssetInfo<XModel>* asset, const unsigned lod)
{
    const auto* model = asset->Asset();
    std::ostringstream ss;
    ss << "model_export/" << model->name << "_lod" << lod << ".OBJ";

    const auto assetFile = context.OpenAssetFile(ss.str());

    if (!assetFile)
        return;

    ObjWriter writer(context.m_zone->m_game->GetShortName(), context.m_zone->m_name);
    DistinctMapper<Material*> materialMapper(model->numsurfs);

    AddObjMaterials(writer, materialMapper, model);
    AddObjObjects(writer, materialMapper, model, lod);
    AddObjVertices(writer, model, lod);
    AddObjFaces(writer, model, lod);

    writer.WriteObj(*assetFile, std::string(model->name) + ".mtl");
}

void AssetDumperXModel::DumpObj(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    const auto* model = asset->Asset();

    DumpObjMat(context, asset);
    for (auto currentLod = 0u; currentLod < model->numLods; currentLod++)
    {
        DumpObjLod(context, asset, currentLod);
    }
}

void AssetDumperXModel::AddXModelBones(const AssetDumpingContext& context, AbstractXModelWriter& writer, const XModel* model)
{
    for (auto boneNum = 0u; boneNum < model->numBones; boneNum++)
    {
        XModelBone bone;
        if (model->boneNames[boneNum] < context.m_zone->m_script_strings.Count())
            bone.name = context.m_zone->m_script_strings[model->boneNames[boneNum]];
        else
            bone.name = "INVALID_BONE_NAME";

        if (boneNum < model->numRootBones)
            bone.parentIndex = -1;
        else
            bone.parentIndex = static_cast<int>(boneNum - static_cast<unsigned int>(model->parentList[boneNum - model->numRootBones]));

        bone.scale[0] = 1.0f;
        bone.scale[1] = 1.0f;
        bone.scale[2] = 1.0f;

        bone.globalOffset[0] = model->baseMat[boneNum].trans[0];
        bone.globalOffset[1] = model->baseMat[boneNum].trans[1];
        bone.globalOffset[2] = model->baseMat[boneNum].trans[2];
        bone.globalRotation =
            Quaternion32(model->baseMat[boneNum].quat[0], model->baseMat[boneNum].quat[1], model->baseMat[boneNum].quat[2], model->baseMat[boneNum].quat[3]);

        if (boneNum < model->numRootBones)
        {
            bone.localOffset[0] = 0;
            bone.localOffset[1] = 0;
            bone.localOffset[2] = 0;
            bone.localRotation = Quaternion32(0, 0, 0, 1);
        }
        else
        {
            bone.localOffset[0] = model->trans[boneNum - model->numRootBones][0];
            bone.localOffset[1] = model->trans[boneNum - model->numRootBones][1];
            bone.localOffset[2] = model->trans[boneNum - model->numRootBones][2];
            bone.localRotation = Quaternion32(QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][0]),
                                              QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][1]),
                                              QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][2]),
                                              QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][3]));
        }

        writer.AddBone(std::move(bone));
    }
}

void AssetDumperXModel::AddXModelMaterials(AbstractXModelWriter& writer, DistinctMapper<Material*>& materialMapper, const XModel* model)
{
    for (auto surfaceMaterialNum = 0; surfaceMaterialNum < model->numsurfs; surfaceMaterialNum++)
    {
        Material* material = model->materialHandles[surfaceMaterialNum];
        if (materialMapper.Add(material))
        {
            XModelMaterial xMaterial;
            xMaterial.ApplyDefaults();

            xMaterial.name = material->info.name;
            const auto* colorMap = GetMaterialColorMap(material);
            if (colorMap)
                xMaterial.colorMapName = std::string(colorMap->name);

            writer.AddMaterial(std::move(xMaterial));
        }
    }
}

void AssetDumperXModel::AddXModelObjects(AbstractXModelWriter& writer, const XModel* model, const unsigned lod)
{
    const auto surfCount = model->lodInfo[lod].numsurfs;

    for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
    {
        XModelObject object;
        object.name = "surf" + std::to_string(surfIndex);

        writer.AddObject(std::move(object));
    }
}

void AssetDumperXModel::AddXModelVertices(AbstractXModelWriter& writer, const XModel* model, const unsigned lod)
{
    const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
    const auto surfCount = model->lodInfo[lod].numsurfs;

    for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
    {
        const auto& surface = surfs[surfIndex];

        for (auto vertexIndex = 0u; vertexIndex < surface.vertCount; vertexIndex++)
        {
            const auto& v = surface.verts0[vertexIndex];
            vec2_t uv;
            vec3_t normalVec;
            vec4_t color;

            Common::Vec2UnpackTexCoords(v.texCoord, &uv);
            Common::Vec3UnpackUnitVec(v.normal, &normalVec);
            Common::Vec4UnpackGfxColor(v.color, &color);

            XModelVertex vertex{};
            vertex.coordinates[0] = v.xyz[0];
            vertex.coordinates[1] = v.xyz[1];
            vertex.coordinates[2] = v.xyz[2];
            vertex.normal[0] = normalVec[0];
            vertex.normal[1] = normalVec[1];
            vertex.normal[2] = normalVec[2];
            vertex.color[0] = color[0];
            vertex.color[1] = color[1];
            vertex.color[2] = color[2];
            vertex.color[3] = color[3];
            vertex.uv[0] = uv[0];
            vertex.uv[1] = uv[1];

            writer.AddVertex(vertex);
        }
    }
}

void AssetDumperXModel::AllocateXModelBoneWeights(const XModel* model, const unsigned lod, XModelVertexBoneWeightCollection& weightCollection)
{
    const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
    const auto surfCount = model->lodInfo[lod].numsurfs;

    weightCollection.totalWeightCount = 0u;
    for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
    {
        const auto& surface = surfs[surfIndex];

        if (surface.vertList)
        {
            weightCollection.totalWeightCount += surface.vertListCount;
        }

        if (surface.vertInfo.vertsBlend)
        {
            weightCollection.totalWeightCount += surface.vertInfo.vertCount[0] * 1;
            weightCollection.totalWeightCount += surface.vertInfo.vertCount[1] * 2;
            weightCollection.totalWeightCount += surface.vertInfo.vertCount[2] * 3;
            weightCollection.totalWeightCount += surface.vertInfo.vertCount[3] * 4;
        }
    }

    weightCollection.weights = std::make_unique<XModelBoneWeight[]>(weightCollection.totalWeightCount);
}

void AssetDumperXModel::AddXModelVertexBoneWeights(AbstractXModelWriter& writer,
                                                   const XModel* model,
                                                   const unsigned lod,
                                                   XModelVertexBoneWeightCollection& weightCollection)
{
    const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
    const auto surfCount = model->lodInfo[lod].numsurfs;

    size_t weightOffset = 0u;

    for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
    {
        const auto& surface = surfs[surfIndex];
        auto handledVertices = 0u;

        if (surface.vertList)
        {
            for (auto vertListIndex = 0u; vertListIndex < surface.vertListCount; vertListIndex++)
            {
                const auto& vertList = surface.vertList[vertListIndex];
                const auto* boneWeightOffset = &weightCollection.weights[weightOffset];

                weightCollection.weights[weightOffset++] = XModelBoneWeight{static_cast<int>(vertList.boneOffset / sizeof(DObjSkelMat)), 1.0f};

                for (auto vertListVertexOffset = 0u; vertListVertexOffset < vertList.vertCount; vertListVertexOffset++)
                {
                    writer.AddVertexBoneWeights(XModelVertexBoneWeights{boneWeightOffset, 1});
                }
                handledVertices += vertList.vertCount;
            }
        }

        auto vertsBlendOffset = 0u;
        if (surface.vertInfo.vertsBlend)
        {
            // 1 bone weight
            for (auto vertIndex = 0; vertIndex < surface.vertInfo.vertCount[0]; vertIndex++)
            {
                const auto* boneWeightOffset = &weightCollection.weights[weightOffset];
                const auto boneIndex0 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 0] / sizeof(DObjSkelMat));
                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex0, 1.0f};

                vertsBlendOffset += 1;

                writer.AddVertexBoneWeights(XModelVertexBoneWeights{boneWeightOffset, 1});
            }

            // 2 bone weights
            for (auto vertIndex = 0; vertIndex < surface.vertInfo.vertCount[1]; vertIndex++)
            {
                const auto* boneWeightOffset = &weightCollection.weights[weightOffset];
                const auto boneIndex0 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 0] / sizeof(DObjSkelMat));
                const auto boneIndex1 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 1] / sizeof(DObjSkelMat));
                const auto boneWeight1 = HalfFloat::ToFloat(surface.vertInfo.vertsBlend[vertsBlendOffset + 2]);
                const auto boneWeight0 = 1.0f - boneWeight1;

                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex0, boneWeight0};
                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex1, boneWeight1};

                vertsBlendOffset += 3;

                writer.AddVertexBoneWeights(XModelVertexBoneWeights{boneWeightOffset, 2});
            }

            // 3 bone weights
            for (auto vertIndex = 0; vertIndex < surface.vertInfo.vertCount[2]; vertIndex++)
            {
                const auto* boneWeightOffset = &weightCollection.weights[weightOffset];
                const auto boneIndex0 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 0] / sizeof(DObjSkelMat));
                const auto boneIndex1 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 1] / sizeof(DObjSkelMat));
                const auto boneWeight1 = HalfFloat::ToFloat(surface.vertInfo.vertsBlend[vertsBlendOffset + 2]);
                const auto boneIndex2 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 3] / sizeof(DObjSkelMat));
                const auto boneWeight2 = HalfFloat::ToFloat(surface.vertInfo.vertsBlend[vertsBlendOffset + 4]);
                const auto boneWeight0 = 1.0f - boneWeight1 - boneWeight2;

                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex0, boneWeight0};
                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex1, boneWeight1};
                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex2, boneWeight2};

                vertsBlendOffset += 5;

                writer.AddVertexBoneWeights(XModelVertexBoneWeights{boneWeightOffset, 3});
            }

            // 4 bone weights
            for (auto vertIndex = 0; vertIndex < surface.vertInfo.vertCount[3]; vertIndex++)
            {
                const auto* boneWeightOffset = &weightCollection.weights[weightOffset];
                const auto boneIndex0 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 0] / sizeof(DObjSkelMat));
                const auto boneIndex1 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 1] / sizeof(DObjSkelMat));
                const auto boneWeight1 = HalfFloat::ToFloat(surface.vertInfo.vertsBlend[vertsBlendOffset + 2]);
                const auto boneIndex2 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 3] / sizeof(DObjSkelMat));
                const auto boneWeight2 = HalfFloat::ToFloat(surface.vertInfo.vertsBlend[vertsBlendOffset + 4]);
                const auto boneIndex3 = static_cast<int>(surface.vertInfo.vertsBlend[vertsBlendOffset + 5] / sizeof(DObjSkelMat));
                const auto boneWeight3 = HalfFloat::ToFloat(surface.vertInfo.vertsBlend[vertsBlendOffset + 6]);
                const auto boneWeight0 = 1.0f - boneWeight1 - boneWeight2 - boneWeight3;

                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex0, boneWeight0};
                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex1, boneWeight1};
                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex2, boneWeight2};
                weightCollection.weights[weightOffset++] = XModelBoneWeight{boneIndex3, boneWeight3};

                vertsBlendOffset += 7;

                writer.AddVertexBoneWeights(XModelVertexBoneWeights{boneWeightOffset, 4});
            }

            handledVertices += surface.vertInfo.vertCount[0] + surface.vertInfo.vertCount[1] + surface.vertInfo.vertCount[2] + surface.vertInfo.vertCount[3];
        }

        for (; handledVertices < surface.vertCount; handledVertices++)
        {
            writer.AddVertexBoneWeights(XModelVertexBoneWeights{nullptr, 0});
        }
    }
}

void AssetDumperXModel::AddXModelFaces(AbstractXModelWriter& writer, const DistinctMapper<Material*>& materialMapper, const XModel* model, const unsigned lod)
{
    const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
    const auto surfCount = model->lodInfo[lod].numsurfs;
    const auto baseSurfIndex = model->lodInfo[lod].surfIndex;

    for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
    {
        const auto& surface = surfs[surfIndex];
        for (auto triIndex = 0u; triIndex < surface.triCount; triIndex++)
        {
            const auto& tri = surface.triIndices[triIndex];

            XModelFace face{};
            face.vertexIndex[0] = tri[0] + surface.baseVertIndex;
            face.vertexIndex[1] = tri[1] + surface.baseVertIndex;
            face.vertexIndex[2] = tri[2] + surface.baseVertIndex;
            face.objectIndex = static_cast<int>(surfIndex);
            face.materialIndex = static_cast<int>(materialMapper.GetDistinctPositionByInputPosition(surfIndex + baseSurfIndex));
            writer.AddFace(face);
        }
    }
}

void AssetDumperXModel::DumpXModelExportLod(const AssetDumpingContext& context, XAssetInfo<XModel>* asset, const unsigned lod)
{
    const auto* model = asset->Asset();

    std::ostringstream ss;
    ss << "model_export/" << model->name << "_lod" << lod << ".XMODEL_EXPORT";

    const auto assetFile = context.OpenAssetFile(ss.str());

    if (!assetFile)
        return;

    const auto writer = XModelExportWriter::CreateWriterForVersion6(context.m_zone->m_game->GetShortName(), context.m_zone->m_name);
    DistinctMapper<Material*> materialMapper(model->numsurfs);
    XModelVertexBoneWeightCollection boneWeightCollection;
    AllocateXModelBoneWeights(model, lod, boneWeightCollection);

    AddXModelBones(context, *writer, model);
    AddXModelMaterials(*writer, materialMapper, model);
    AddXModelObjects(*writer, model, lod);
    AddXModelVertices(*writer, model, lod);
    AddXModelVertexBoneWeights(*writer, model, lod, boneWeightCollection);
    AddXModelFaces(*writer, materialMapper, model, lod);

    writer->Write(*assetFile);
}

void AssetDumperXModel::DumpXModelExport(const AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    const auto* model = asset->Asset();
    for (auto currentLod = 0u; currentLod < model->numLods; currentLod++)
    {
        DumpXModelExportLod(context, asset, currentLod);
    }
}

void AssetDumperXModel::DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    switch (ObjWriting::Configuration.ModelOutputFormat)
    {
    case ObjWriting::Configuration_t::ModelOutputFormat_e::OBJ:
        DumpObj(context, asset);
        break;

    case ObjWriting::Configuration_t::ModelOutputFormat_e::XMODEL_EXPORT:
        DumpXModelExport(context, asset);
        break;

    default:
        assert(false);
        break;
    }
}
