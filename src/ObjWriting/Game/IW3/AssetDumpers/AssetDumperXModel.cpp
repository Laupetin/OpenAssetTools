#include "AssetDumperXModel.h"

#include "Game/IW3/CommonIW3.h"
#include "ObjWriting.h"
#include "Utils/DistinctMapper.h"
#include "Utils/HalfFloat.h"
#include "Utils/QuatInt16.h"
#include "XModel/Export/XModelBinWriter.h"
#include "XModel/Export/XModelExportWriter.h"
#include "XModel/Gltf/GltfBinOutput.h"
#include "XModel/Gltf/GltfTextOutput.h"
#include "XModel/Gltf/GltfWriter.h"
#include "XModel/Obj/ObjWriter.h"
#include "XModel/XModelWriter.h"

#include <cassert>
#include <format>

using namespace IW3;

namespace
{
    std::string GetFileNameForLod(const std::string& modelName, const unsigned lod, const std::string& extension)
    {
        return std::format("model_export/{}_lod{}{}", modelName, lod, extension);
    }

    GfxImage* GetMaterialColorMap(const Material* material)
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

    GfxImage* GetMaterialNormalMap(const Material* material)
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

    GfxImage* GetMaterialSpecularMap(const Material* material)
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

    void AddXModelBones(XModelCommon& out, const AssetDumpingContext& context, const XModel* model)
    {
        for (auto boneNum = 0u; boneNum < model->numBones; boneNum++)
        {
            XModelBone bone;
            if (model->boneNames[boneNum] < context.m_zone.m_script_strings.Count())
                bone.name = context.m_zone.m_script_strings[model->boneNames[boneNum]];
            else
                bone.name = "INVALID_BONE_NAME";

            if (boneNum >= model->numRootBones)
                bone.parentIndex = boneNum - static_cast<unsigned int>(model->parentList[boneNum - model->numRootBones]);
            else
                bone.parentIndex = std::nullopt;

            bone.scale[0] = 1.0f;
            bone.scale[1] = 1.0f;
            bone.scale[2] = 1.0f;

            bone.globalOffset[0] = model->baseMat[boneNum].trans[0];
            bone.globalOffset[1] = model->baseMat[boneNum].trans[1];
            bone.globalOffset[2] = model->baseMat[boneNum].trans[2];
            bone.globalRotation = {
                .x = model->baseMat[boneNum].quat[0],
                .y = model->baseMat[boneNum].quat[1],
                .z = model->baseMat[boneNum].quat[2],
                .w = model->baseMat[boneNum].quat[3],
            };

            if (boneNum < model->numRootBones)
            {
                bone.localOffset[0] = 0;
                bone.localOffset[1] = 0;
                bone.localOffset[2] = 0;
                bone.localRotation = {.x = 0, .y = 0, .z = 0, .w = 1};
            }
            else
            {
                bone.localOffset[0] = model->trans[boneNum - model->numRootBones][0];
                bone.localOffset[1] = model->trans[boneNum - model->numRootBones][1];
                bone.localOffset[2] = model->trans[boneNum - model->numRootBones][2];
                bone.localRotation = {
                    .x = QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][0]),
                    .y = QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][1]),
                    .z = QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][2]),
                    .w = QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][3]),
                };
            }

            out.m_bones.emplace_back(std::move(bone));
        }
    }

    const char* AssetName(const char* input)
    {
        if (input && input[0] == ',')
            return &input[1];

        return input;
    }

    void AddXModelMaterials(XModelCommon& out, DistinctMapper<Material*>& materialMapper, const XModel* model)
    {
        for (auto surfaceMaterialNum = 0u; surfaceMaterialNum < model->numsurfs; surfaceMaterialNum++)
        {
            Material* material = model->materialHandles[surfaceMaterialNum];
            if (materialMapper.Add(material))
            {
                XModelMaterial xMaterial;
                xMaterial.ApplyDefaults();

                xMaterial.name = AssetName(material->info.name);
                const auto* colorMap = GetMaterialColorMap(material);
                if (colorMap)
                    xMaterial.colorMapName = AssetName(colorMap->name);

                const auto* normalMap = GetMaterialNormalMap(material);
                if (normalMap)
                    xMaterial.normalMapName = AssetName(normalMap->name);

                const auto* specularMap = GetMaterialSpecularMap(material);
                if (specularMap)
                    xMaterial.specularMapName = AssetName(specularMap->name);

                out.m_materials.emplace_back(std::move(xMaterial));
            }
        }
    }

    void AddXModelObjects(XModelCommon& out, const XModel* model, const unsigned lod, const DistinctMapper<Material*>& materialMapper)
    {
        const auto surfCount = model->lodInfo[lod].numsurfs;
        const auto baseSurfaceIndex = model->lodInfo[lod].surfIndex;

        for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
        {
            XModelObject object;
            object.name = std::format("surf{}", surfIndex);
            object.materialIndex = static_cast<int>(materialMapper.GetDistinctPositionByInputPosition(surfIndex + baseSurfaceIndex));

            out.m_objects.emplace_back(std::move(object));
        }
    }

    void AddXModelVertices(XModelCommon& out, const XModel* model, const unsigned lod)
    {
        const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
        const auto surfCount = model->lodInfo[lod].numsurfs;

        for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
        {
            const auto& surface = surfs[surfIndex];

            for (auto vertexIndex = 0u; vertexIndex < surface.vertCount; vertexIndex++)
            {
                const auto& v = surface.verts0[vertexIndex];

                XModelVertex vertex{};
                vertex.coordinates[0] = v.xyz[0];
                vertex.coordinates[1] = v.xyz[1];
                vertex.coordinates[2] = v.xyz[2];
                Common::Vec3UnpackUnitVec(v.normal, vertex.normal);
                Common::Vec4UnpackGfxColor(v.color, vertex.color);
                Common::Vec2UnpackTexCoords(v.texCoord, vertex.uv);

                out.m_vertices.emplace_back(vertex);
            }
        }
    }

    void AllocateXModelBoneWeights(const XModel* model, const unsigned lod, XModelVertexBoneWeightCollection& weightCollection)
    {
        const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
        const auto surfCount = model->lodInfo[lod].numsurfs;

        auto totalWeightCount = 0u;
        for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
        {
            const auto& surface = surfs[surfIndex];

            if (surface.vertList)
            {
                totalWeightCount += surface.vertListCount;
            }

            if (surface.vertInfo.vertsBlend)
            {
                totalWeightCount += surface.vertInfo.vertCount[0] * 1;
                totalWeightCount += surface.vertInfo.vertCount[1] * 2;
                totalWeightCount += surface.vertInfo.vertCount[2] * 3;
                totalWeightCount += surface.vertInfo.vertCount[3] * 4;
            }
        }

        weightCollection.weights.resize(totalWeightCount);
    }

    float BoneWeight16(const uint16_t value)
    {
        return static_cast<float>(value) / static_cast<float>(std::numeric_limits<uint16_t>::max());
    }

    void AddXModelVertexBoneWeights(XModelCommon& out, const XModel* model, const unsigned lod)
    {
        const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
        const auto surfCount = model->lodInfo[lod].numsurfs;
        auto& weightCollection = out.m_bone_weight_data;

        auto weightOffset = 0u;

        for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
        {
            const auto& surface = surfs[surfIndex];
            auto handledVertices = 0u;

            if (surface.vertList)
            {
                for (auto vertListIndex = 0u; vertListIndex < surface.vertListCount; vertListIndex++)
                {
                    const auto& vertList = surface.vertList[vertListIndex];
                    const auto boneWeightOffset = weightOffset;

                    weightCollection.weights[weightOffset++] =
                        XModelBoneWeight{.boneIndex = static_cast<unsigned>(vertList.boneOffset / sizeof(DObjSkelMat)), .weight = 1.0f};

                    for (auto vertListVertexOffset = 0u; vertListVertexOffset < vertList.vertCount; vertListVertexOffset++)
                    {
                        out.m_vertex_bone_weights.emplace_back(boneWeightOffset, 1);
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
                    const auto boneWeightOffset = weightOffset;
                    const unsigned boneIndex0 = surface.vertInfo.vertsBlend[vertsBlendOffset + 0] / sizeof(DObjSkelMat);
                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex0, .weight = 1.0f};

                    vertsBlendOffset += 1;

                    out.m_vertex_bone_weights.emplace_back(boneWeightOffset, 1);
                }

                // 2 bone weights
                for (auto vertIndex = 0; vertIndex < surface.vertInfo.vertCount[1]; vertIndex++)
                {
                    const auto boneWeightOffset = weightOffset;
                    const unsigned boneIndex0 = surface.vertInfo.vertsBlend[vertsBlendOffset + 0] / sizeof(DObjSkelMat);
                    const unsigned boneIndex1 = surface.vertInfo.vertsBlend[vertsBlendOffset + 1] / sizeof(DObjSkelMat);
                    const auto boneWeight1 = BoneWeight16(surface.vertInfo.vertsBlend[vertsBlendOffset + 2]);
                    const auto boneWeight0 = 1.0f - boneWeight1;

                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex0, .weight = boneWeight0};
                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex1, .weight = boneWeight1};

                    vertsBlendOffset += 3;

                    out.m_vertex_bone_weights.emplace_back(boneWeightOffset, 2);
                }

                // 3 bone weights
                for (auto vertIndex = 0; vertIndex < surface.vertInfo.vertCount[2]; vertIndex++)
                {
                    const auto boneWeightOffset = weightOffset;
                    const unsigned boneIndex0 = surface.vertInfo.vertsBlend[vertsBlendOffset + 0] / sizeof(DObjSkelMat);
                    const unsigned boneIndex1 = surface.vertInfo.vertsBlend[vertsBlendOffset + 1] / sizeof(DObjSkelMat);
                    const auto boneWeight1 = BoneWeight16(surface.vertInfo.vertsBlend[vertsBlendOffset + 2]);
                    const unsigned boneIndex2 = surface.vertInfo.vertsBlend[vertsBlendOffset + 3] / sizeof(DObjSkelMat);
                    const auto boneWeight2 = BoneWeight16(surface.vertInfo.vertsBlend[vertsBlendOffset + 4]);
                    const auto boneWeight0 = 1.0f - boneWeight1 - boneWeight2;

                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex0, .weight = boneWeight0};
                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex1, .weight = boneWeight1};
                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex2, .weight = boneWeight2};

                    vertsBlendOffset += 5;

                    out.m_vertex_bone_weights.emplace_back(boneWeightOffset, 3);
                }

                // 4 bone weights
                for (auto vertIndex = 0; vertIndex < surface.vertInfo.vertCount[3]; vertIndex++)
                {
                    const auto boneWeightOffset = weightOffset;
                    const unsigned boneIndex0 = surface.vertInfo.vertsBlend[vertsBlendOffset + 0] / sizeof(DObjSkelMat);
                    const unsigned boneIndex1 = surface.vertInfo.vertsBlend[vertsBlendOffset + 1] / sizeof(DObjSkelMat);
                    const auto boneWeight1 = BoneWeight16(surface.vertInfo.vertsBlend[vertsBlendOffset + 2]);
                    const unsigned boneIndex2 = surface.vertInfo.vertsBlend[vertsBlendOffset + 3] / sizeof(DObjSkelMat);
                    const auto boneWeight2 = BoneWeight16(surface.vertInfo.vertsBlend[vertsBlendOffset + 4]);
                    const unsigned boneIndex3 = surface.vertInfo.vertsBlend[vertsBlendOffset + 5] / sizeof(DObjSkelMat);
                    const auto boneWeight3 = BoneWeight16(surface.vertInfo.vertsBlend[vertsBlendOffset + 6]);
                    const auto boneWeight0 = 1.0f - boneWeight1 - boneWeight2 - boneWeight3;

                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex0, .weight = boneWeight0};
                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex1, .weight = boneWeight1};
                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex2, .weight = boneWeight2};
                    weightCollection.weights[weightOffset++] = XModelBoneWeight{.boneIndex = boneIndex3, .weight = boneWeight3};

                    vertsBlendOffset += 7;

                    out.m_vertex_bone_weights.emplace_back(boneWeightOffset, 4);
                }

                handledVertices +=
                    surface.vertInfo.vertCount[0] + surface.vertInfo.vertCount[1] + surface.vertInfo.vertCount[2] + surface.vertInfo.vertCount[3];
            }

            for (; handledVertices < surface.vertCount; handledVertices++)
            {
                out.m_vertex_bone_weights.emplace_back(0, 0);
            }
        }
    }

    void AddXModelFaces(XModelCommon& out, const XModel* model, const unsigned lod)
    {
        const auto* surfs = &model->surfs[model->lodInfo[lod].surfIndex];
        const auto surfCount = model->lodInfo[lod].numsurfs;

        for (auto surfIndex = 0u; surfIndex < surfCount; surfIndex++)
        {
            const auto& surface = surfs[surfIndex];
            auto& object = out.m_objects[surfIndex];
            object.m_faces.reserve(surface.triCount);

            for (auto triIndex = 0u; triIndex < surface.triCount; triIndex++)
            {
                const auto& tri = surface.triIndices[triIndex];

                XModelFace face{};
                face.vertexIndex[0] = tri[0] + surface.baseVertIndex;
                face.vertexIndex[1] = tri[1] + surface.baseVertIndex;
                face.vertexIndex[2] = tri[2] + surface.baseVertIndex;
                object.m_faces.emplace_back(face);
            }
        }
    }

    void PopulateXModelWriter(XModelCommon& out, const AssetDumpingContext& context, const unsigned lod, const XModel* model)
    {
        DistinctMapper<Material*> materialMapper(model->numsurfs);
        AllocateXModelBoneWeights(model, lod, out.m_bone_weight_data);

        out.m_name = std::format("{}_lod{}", model->name, lod);
        AddXModelBones(out, context, model);
        AddXModelMaterials(out, materialMapper, model);
        AddXModelObjects(out, model, lod, materialMapper);
        AddXModelVertices(out, model, lod);
        AddXModelVertexBoneWeights(out, model, lod);
        AddXModelFaces(out, model, lod);
    }

    void DumpObjMtl(const XModelCommon& common, const AssetDumpingContext& context, const XAssetInfo<XModel>* asset)
    {
        const auto* model = asset->Asset();
        const auto mtlFile = context.OpenAssetFile(std::format("model_export/{}.mtl", model->name));

        if (!mtlFile)
            return;

        const auto writer = obj::CreateMtlWriter(*mtlFile, context.m_zone.m_game->GetShortName(), context.m_zone.m_name);
        DistinctMapper<Material*> materialMapper(model->numsurfs);

        writer->Write(common);
    }

    void DumpObjLod(const XModelCommon& common, const AssetDumpingContext& context, const XAssetInfo<XModel>* asset, const unsigned lod)
    {
        const auto* model = asset->Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForLod(model->name, lod, ".obj"));

        if (!assetFile)
            return;

        const auto writer = obj::CreateObjWriter(*assetFile, std::format("{}.mtl", model->name), context.m_zone.m_game->GetShortName(), context.m_zone.m_name);
        DistinctMapper<Material*> materialMapper(model->numsurfs);

        writer->Write(common);
    }

    void DumpXModelExportLod(const XModelCommon& common, const AssetDumpingContext& context, const XAssetInfo<XModel>* asset, const unsigned lod)
    {
        const auto* model = asset->Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForLod(model->name, lod, ".XMODEL_EXPORT"));

        if (!assetFile)
            return;

        const auto writer = xmodel_export::CreateWriterForVersion6(*assetFile, context.m_zone.m_game->GetShortName(), context.m_zone.m_name);
        writer->Write(common);
    }

    void DumpXModelBinLod(const XModelCommon& common, const AssetDumpingContext& context, const XAssetInfo<XModel>* asset, const unsigned lod)
    {
        const auto* model = asset->Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForLod(model->name, lod, ".xmodel_bin"));

        if (!assetFile)
            return;

        const auto writer = xmodel_bin::CreateWriterForVersion7(*assetFile, context.m_zone.m_game->GetShortName(), context.m_zone.m_name);
        writer->Write(common);
    }

    template<typename T>
    void DumpGltfLod(
        const XModelCommon& common, const AssetDumpingContext& context, const XAssetInfo<XModel>* asset, const unsigned lod, const std::string& extension)
    {
        const auto* model = asset->Asset();
        const auto assetFile = context.OpenAssetFile(GetFileNameForLod(model->name, lod, extension));

        if (!assetFile)
            return;

        const auto output = std::make_unique<T>(*assetFile);
        const auto writer = gltf::Writer::CreateWriter(output.get(), context.m_zone.m_game->GetShortName(), context.m_zone.m_name);

        writer->Write(common);
    }

    void DumpXModelSurfs(const AssetDumpingContext& context, const XAssetInfo<XModel>* asset)
    {
        const auto* model = asset->Asset();

        for (auto currentLod = 0u; currentLod < model->numLods; currentLod++)
        {
            XModelCommon common;
            PopulateXModelWriter(common, context, currentLod, asset->Asset());

            switch (ObjWriting::Configuration.ModelOutputFormat)
            {
            case ObjWriting::Configuration_t::ModelOutputFormat_e::OBJ:
                DumpObjLod(common, context, asset, currentLod);
                if (currentLod == 0u)
                    DumpObjMtl(common, context, asset);
                break;

            case ObjWriting::Configuration_t::ModelOutputFormat_e::XMODEL_EXPORT:
                DumpXModelExportLod(common, context, asset, currentLod);
                break;

            case ObjWriting::Configuration_t::ModelOutputFormat_e::XMODEL_BIN:
                DumpXModelBinLod(common, context, asset, currentLod);
                break;

            case ObjWriting::Configuration_t::ModelOutputFormat_e::GLTF:
                DumpGltfLod<gltf::TextOutput>(common, context, asset, currentLod, ".gltf");
                break;

            case ObjWriting::Configuration_t::ModelOutputFormat_e::GLB:
                DumpGltfLod<gltf::BinOutput>(common, context, asset, currentLod, ".glb");
                break;

            default:
                assert(false);
                break;
            }
        }
    }
} // namespace

bool AssetDumperXModel::ShouldDump(XAssetInfo<XModel>* asset)
{
    return !asset->m_name.empty() && asset->m_name[0] != ',';
}

void AssetDumperXModel::DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    DumpXModelSurfs(context, asset);
}
