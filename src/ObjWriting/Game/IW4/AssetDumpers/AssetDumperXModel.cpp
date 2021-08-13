#include "AssetDumperXModel.h"

#include <cassert>
#include <set>
#include <iomanip>

#include "ObjWriting.h"
#include "Game/IW4/CommonIW4.h"
#include "Math/Quaternion.h"
#include "Model/XModel/XModelExportWriter.h"
#include "Utils/HalfFloat.h"
#include "Utils/QuatInt16.h"

using namespace IW4;

bool AssetDumperXModel::ShouldDump(XAssetInfo<XModel>* asset)
{
    return !asset->m_name.empty() && asset->m_name[0] != ',';
}

void AssetDumperXModel::DumpObjMatMaterial(AssetDumpingContext& context, const Material* material, std::ostream& stream)
{
    stream << "\n";
    stream << "newmtl " << material->info.name << "\n";

    GfxImage* colorMap = nullptr;
    GfxImage* normalMap = nullptr;
    GfxImage* specularMap = nullptr;

    for (auto i = 0u; i < material->textureCount; i++)
    {
        const auto& texture = material->textureTable[i];

        switch (texture.semantic)
        {
        case TS_COLOR_MAP:
            colorMap = texture.u.image;
            break;

        case TS_NORMAL_MAP:
            normalMap = texture.u.image;
            break;

        case TS_SPECULAR_MAP:
            specularMap = texture.u.image;
            break;

        default:
            break;
        }
    }

    if (colorMap)
        stream << "map_Kd " << colorMap->name << ".dds\n";

    if (normalMap)
        stream << "map_bump " << normalMap->name << ".dds\n";

    if (specularMap)
        stream << "map_Ks " << specularMap->name << ".dds\n";
}

void AssetDumperXModel::DumpObjMat(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    const auto* model = asset->Asset();
    const auto matFile = context.OpenAssetFile("xmodelsurfs/" + std::string(model->name) + ".mtl");

    if (!matFile)
        return;

    auto& stream = *matFile;
    stream << "# OpenAssetTools MAT File (IW4)\n";

    if (model->numsurfs == 0 || model->materialHandles == nullptr)
        return;

    std::set<Material*> uniqueMaterials;
    for (auto i = 0u; i < model->numsurfs; i++)
    {
        if (model->materialHandles[i] != nullptr)
            uniqueMaterials.emplace(model->materialHandles[i]);
    }

    stream << "# Material count: " << uniqueMaterials.size() << "\n";

    for (const auto* material : uniqueMaterials)
    {
        DumpObjMatMaterial(context, material, stream);
    }
}

void AssetDumperXModel::DumpObjLod(AssetDumpingContext& context, XAssetInfo<XModel>* asset, const unsigned lod)
{
    const auto* model = asset->Asset();
    const auto* modelSurfs = model->lodInfo[lod].modelSurfs;
    const auto assetFile = context.OpenAssetFile("xmodelsurfs/" + std::string(modelSurfs->name) + ".obj");

    if (!assetFile)
        return;

    auto& stream = *assetFile;
    stream << "# OpenAssetTools OBJ File (IW4)\n";

    stream << "mtllib " << model->name << ".mtl\n";

    if (model->lodInfo[lod].modelSurfs == nullptr || model->lodInfo[lod].modelSurfs->surfs == nullptr)
        return;

    for (auto i = 0; i < model->lodInfo[lod].numsurfs; i++)
    {
        const auto* surf = &modelSurfs->surfs[i];

        stream << "o surf" << i << "\n";

        for (auto vi = 0; vi < surf->vertCount; vi++)
        {
            const auto* vertex = &surf->verts0[vi];
            stream << "v " << vertex->xyz[0] << " " << vertex->xyz[2] << " " << -vertex->xyz[1] << "\n";
        }

        stream << "\n";

        for (auto vi = 0; vi < surf->vertCount; vi++)
        {
            const auto* vertex = &surf->verts0[vi];
            vec2_t texCoords;
            Common::Vec2UnpackTexCoords(vertex->texCoord, &texCoords);

            stream << "vt " << texCoords[0] << " " << (1.0f - texCoords[1]) << "\n";
        }

        stream << "\n";

        for (auto vi = 0; vi < surf->vertCount; vi++)
        {
            const auto* vertex = &surf->verts0[vi];
            vec3_t normalVec;
            Common::Vec3UnpackUnitVec(vertex->normal, &normalVec);

            stream << "vn " << normalVec[0] << " " << normalVec[2] << " " << -normalVec[1] << "\n";
        }

        stream << "\n";

        if (model->numsurfs > i && model->materialHandles && model->materialHandles[i])
        {
            stream << "usemtl " << model->materialHandles[i]->info.name << "\n";
        }

        stream << "\n";

        for (auto ti = 0; ti < surf->triCount; ti++)
        {
            const auto* indices = reinterpret_cast<r_index16_t*>(surf->triIndices);

            const auto i0 = surf->baseVertIndex + indices[ti * 3 + 0] + 1;
            const auto i1 = surf->baseVertIndex + indices[ti * 3 + 1] + 1;
            const auto i2 = surf->baseVertIndex + indices[ti * 3 + 2] + 1;

            stream << "f " << i2 << "/" << i2 << "/" << i2
                << " " << i1 << "/" << i1 << "/" << i1
                << " " << i0 << "/" << i0 << "/" << i0
                << "\n";
        }
    }
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

void AssetDumperXModel::AddBonesToWriter(const AssetDumpingContext& context, AbstractXModelWriter& writer, const XModel* model)
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

        if (boneNum < model->numRootBones)
        {
            bone.offset[0] = 0;
            bone.offset[1] = 0;
            bone.offset[2] = 0;
            bone.rotation = Quaternion32(0, 0, 0, 1);
        }
        else
        {
            bone.offset[0] = model->trans[boneNum - model->numRootBones][0];
            bone.offset[1] = model->trans[boneNum - model->numRootBones][1];
            bone.offset[2] = model->trans[boneNum - model->numRootBones][2];
            bone.rotation = Quaternion32(
                QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][0]),
                QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][1]),
                QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][2]),
                QuatInt16::ToFloat(model->quats[boneNum - model->numRootBones][3])
            );
        }

        writer.AddBone(std::move(bone));
    }
}

void AssetDumperXModel::AddMaterialsToWriter(AbstractXModelWriter& writer, DistinctMapper<Material*>& materialMapper, const XModel* model)
{
    for (auto surfaceMaterialNum = 0; surfaceMaterialNum < model->numsurfs; surfaceMaterialNum++)
    {
        Material* material = model->materialHandles[surfaceMaterialNum];
        if (materialMapper.Add(material))
        {
            XModelMaterial xMaterial;
            xMaterial.ApplyDefaults();

            xMaterial.name = material->info.name;

            for (auto textureIndex = 0; textureIndex < material->textureCount; textureIndex++)
            {
                const auto* textureTableEntry = &material->textureTable[textureIndex];
                if (textureTableEntry->semantic == TS_COLOR_MAP && textureTableEntry->u.image)
                {
                    xMaterial.colorMapName = textureTableEntry->u.image->name;
                    break;
                }
            }

            writer.AddMaterial(std::move(xMaterial));
        }
    }
}

void AssetDumperXModel::AddObjectsToWriter(AbstractXModelWriter& writer, const XModelSurfs* modelSurfs)
{
    for (auto surfIndex = 0u; surfIndex < modelSurfs->numsurfs; surfIndex++)
    {
        XModelObject object;
        object.name = "surf" + std::to_string(surfIndex);

        writer.AddObject(std::move(object));
    }
}

void AssetDumperXModel::AddVerticesToWriter(AbstractXModelWriter& writer, const XModelSurfs* modelSurfs)
{
    for (auto surfIndex = 0u; surfIndex < modelSurfs->numsurfs; surfIndex++)
    {
        const auto& surface = modelSurfs->surfs[surfIndex];
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

void AssetDumperXModel::AddFacesToWriter(AbstractXModelWriter& writer, const DistinctMapper<Material*>& materialMapper, const XModelSurfs* modelSurfs,
                                         const int baseSurfaceIndex)
{
    for (auto surfIndex = 0u; surfIndex < modelSurfs->numsurfs; surfIndex++)
    {
        const auto& surface = modelSurfs->surfs[surfIndex];
        for (auto triIndex = 0u; triIndex < surface.triCount; triIndex++)
        {
            const auto& tri = surface.triIndices[triIndex];

            XModelFace face{};
            face.vertexIndex[0] = tri[0] + surface.baseVertIndex;
            face.vertexIndex[1] = tri[1] + surface.baseVertIndex;
            face.vertexIndex[2] = tri[2] + surface.baseVertIndex;
            face.objectIndex = static_cast<int>(surfIndex);
            face.materialIndex = static_cast<int>(materialMapper.GetDistinctPositionByInputPosition(surfIndex + baseSurfaceIndex));
            writer.AddFace(face);
        }
    }
}

void AssetDumperXModel::DumpXModelExportLod(const AssetDumpingContext& context, XAssetInfo<XModel>* asset, const unsigned lod)
{
    const auto* model = asset->Asset();
    const auto* modelSurfs = model->lodInfo[lod].modelSurfs;

    if (modelSurfs->name[0] == ',' || modelSurfs->surfs == nullptr)
        return;

    const auto assetFile = context.OpenAssetFile("model_export/" + std::string(modelSurfs->name) + ".XMODEL_EXPORT");

    if (!assetFile)
        return;

    const auto writer = XModelExportWriter::CreateWriterForVersion6(context.m_zone->m_game->GetShortName(), context.m_zone->m_name);
    DistinctMapper<Material*> materialMapper(model->numsurfs);

    AddBonesToWriter(context, *writer, model);
    AddMaterialsToWriter(*writer, materialMapper, model);
    AddObjectsToWriter(*writer, modelSurfs);
    AddVerticesToWriter(*writer, modelSurfs);
    AddFacesToWriter(*writer, materialMapper, modelSurfs, model->lodInfo[lod].surfIndex);

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
