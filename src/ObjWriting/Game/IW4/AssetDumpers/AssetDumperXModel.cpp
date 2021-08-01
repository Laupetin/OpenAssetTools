#include "AssetDumperXModel.h"

#include <set>

#include "Game/IW4/CommonIW4.h"

using namespace IW4;

bool AssetDumperXModel::ShouldDump(XAssetInfo<XModel>* asset)
{
    return true;
}

void AssetDumperXModel::DumpObjMatMaterial(AssetDumpingContext& context, const Material* material, std::ostream& stream)
{
    stream << "\n";
    stream << "newmtl " << material->info.name << "\n";

    GfxImage* colorMap = nullptr;
    GfxImage* normalMap = nullptr;
    GfxImage* specularMap = nullptr;

    for(auto i = 0u; i < material->textureCount; i++)
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
        if(model->materialHandles[i] != nullptr)
            uniqueMaterials.emplace(model->materialHandles[i]);
    }

    stream << "# Material count: " << uniqueMaterials.size() << "\n";

    for(const auto* material : uniqueMaterials)
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

        if(model->numsurfs > i && model->materialHandles && model->materialHandles[i])
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

void AssetDumperXModel::DumpXModelExportLod(AssetDumpingContext& context, XAssetInfo<XModel>* asset, unsigned lod)
{
}

void AssetDumperXModel::DumpXModelExport(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
}

void AssetDumperXModel::DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    DumpObj(context, asset);
}
