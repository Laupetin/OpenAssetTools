#include "ObjWriter.h"

#include "Game/IW4/CommonIW4.h"

ObjWriter::ObjWriter(std::string gameName, std::string zoneName)
    : m_game_name(std::move(gameName)),
      m_zone_name(std::move(zoneName))
{
}

void ObjWriter::AddObject(ObjObject object)
{
    m_objects.emplace_back(std::move(object));
    m_object_data.emplace_back();
}

void ObjWriter::AddMaterial(MtlMaterial material)
{
    m_materials.emplace_back(std::move(material));
}

void ObjWriter::AddVertex(const int objectId, const ObjVertex vertex)
{
    if (objectId < 0 || static_cast<unsigned>(objectId) >= m_object_data.size())
        return;

    m_object_data[objectId].m_vertices.Add(vertex);
}

void ObjWriter::AddNormal(const int objectId, const ObjNormal normal)
{
    if (objectId < 0 || static_cast<unsigned>(objectId) >= m_object_data.size())
        return;

    m_object_data[objectId].m_normals.Add(normal);
}

void ObjWriter::AddUv(const int objectId, const ObjUv uv)
{
    if (objectId < 0 || static_cast<unsigned>(objectId) >= m_object_data.size())
        return;

    m_object_data[objectId].m_uvs.Add(uv);
}

void ObjWriter::AddFace(const int objectId, const ObjFace face)
{
    if (objectId < 0 || static_cast<unsigned>(objectId) >= m_object_data.size())
        return;

    m_object_data[objectId].m_faces.push_back(face);
}

void ObjWriter::GetObjObjectDataOffsets(std::vector<ObjObjectDataOffsets>& inputOffsets, std::vector<ObjObjectDataOffsets>& distinctOffsets)
{
    ObjObjectDataOffsets currentInputOffsets{};
    ObjObjectDataOffsets currentDistinctOffsets{};

    for (const auto& objectData : m_object_data)
    {
        inputOffsets.push_back(currentInputOffsets);
        distinctOffsets.push_back(currentDistinctOffsets);

        currentInputOffsets.vertexOffset += objectData.m_vertices.GetInputValueCount();
        currentInputOffsets.normalOffset += objectData.m_normals.GetInputValueCount();
        currentInputOffsets.uvOffset += objectData.m_uvs.GetInputValueCount();
        currentDistinctOffsets.vertexOffset += objectData.m_vertices.GetDistinctValueCount();
        currentDistinctOffsets.normalOffset += objectData.m_normals.GetDistinctValueCount();
        currentDistinctOffsets.uvOffset += objectData.m_uvs.GetDistinctValueCount();
    }
}

void ObjWriter::WriteObj(std::ostream& stream)
{
    WriteObj(stream, std::string());
}

void ObjWriter::WriteObj(std::ostream& stream, const std::string& mtlName)
{
    stream << "# OpenAssetTools OBJ File ( " << m_game_name << ")\n";
    stream << "# Game Origin: " << m_game_name << "\n";
    stream << "# Zone Origin: " << m_zone_name << "\n";

    if (!mtlName.empty())
        stream << "mtllib " << mtlName << "\n";

    std::vector<ObjObjectDataOffsets> inputOffsetsByObject;
    std::vector<ObjObjectDataOffsets> distinctOffsetsByObject;
    GetObjObjectDataOffsets(inputOffsetsByObject, distinctOffsetsByObject);

    auto objectIndex = 0;
    for (const auto& object : m_objects)
    {
        const auto& objectData = m_object_data[objectIndex];
        stream << "o " << object.name << "\n";

        for (const auto& v : objectData.m_vertices.GetDistinctValues())
            stream << "v " << v.coordinates[0] << " " << v.coordinates[1] << " " << v.coordinates[2] << "\n";
        for (const auto& uv : objectData.m_uvs.GetDistinctValues())
            stream << "vt " << uv.uv[0] << " " << uv.uv[1] << "\n";
        for (const auto& n : objectData.m_normals.GetDistinctValues())
            stream << "vn " << n.normal[0] << " " << n.normal[1] << " " << n.normal[2] << "\n";

        if (object.materialIndex >= 0 && static_cast<unsigned>(object.materialIndex) < m_materials.size())
            stream << "usemtl " << m_materials[object.materialIndex].materialName << "\n";

        for (const auto& f : objectData.m_faces)
        {
            const size_t v[3]
            {
                objectData.m_vertices.GetDistinctPositionByInputPosition(f.vertexIndex[0] - inputOffsetsByObject[objectIndex].vertexOffset) + distinctOffsetsByObject[objectIndex].vertexOffset + 1,
                objectData.m_vertices.GetDistinctPositionByInputPosition(f.vertexIndex[1] - inputOffsetsByObject[objectIndex].vertexOffset) + distinctOffsetsByObject[objectIndex].vertexOffset + 1,
                objectData.m_vertices.GetDistinctPositionByInputPosition(f.vertexIndex[2] - inputOffsetsByObject[objectIndex].vertexOffset) + distinctOffsetsByObject[objectIndex].vertexOffset + 1
            };
            const size_t n[3]
            {
                objectData.m_normals.GetDistinctPositionByInputPosition(f.normalIndex[0] - inputOffsetsByObject[objectIndex].normalOffset) + distinctOffsetsByObject[objectIndex].normalOffset + 1,
                objectData.m_normals.GetDistinctPositionByInputPosition(f.normalIndex[1] - inputOffsetsByObject[objectIndex].normalOffset) + distinctOffsetsByObject[objectIndex].normalOffset + 1,
                objectData.m_normals.GetDistinctPositionByInputPosition(f.normalIndex[2] - inputOffsetsByObject[objectIndex].normalOffset) + distinctOffsetsByObject[objectIndex].normalOffset + 1
            };
            const size_t uv[3]
            {
                objectData.m_uvs.GetDistinctPositionByInputPosition(f.uvIndex[0] - inputOffsetsByObject[objectIndex].uvOffset) + distinctOffsetsByObject[objectIndex].uvOffset + 1,
                objectData.m_uvs.GetDistinctPositionByInputPosition(f.uvIndex[1] - inputOffsetsByObject[objectIndex].uvOffset) + distinctOffsetsByObject[objectIndex].uvOffset + 1,
                objectData.m_uvs.GetDistinctPositionByInputPosition(f.uvIndex[2] - inputOffsetsByObject[objectIndex].uvOffset) + distinctOffsetsByObject[objectIndex].uvOffset + 1
            };

            stream << "f " << v[0] << "/" << uv[0] << "/" << n[0]
                << " " << v[1] << "/" << uv[1] << "/" << n[1]
                << " " << v[2] << "/" << uv[2] << "/" << n[2]
                << "\n";
        }

        objectIndex++;
    }
}

void ObjWriter::WriteMtl(std::ostream& stream)
{
    stream << "# OpenAssetTools MAT File ( " << m_game_name << ")\n";
    stream << "# Game Origin: " << m_game_name << "\n";
    stream << "# Zone Origin: " << m_zone_name << "\n";
    stream << "# Material count: " << m_materials.size() << "\n";

    for (const auto& material : m_materials)
    {
        stream << "\n";
        stream << "newmtl " << material.materialName << "\n";

        if (!material.colorMapName.empty())
            stream << "map_Kd ../images/" << material.colorMapName << ".dds\n";

        if (!material.normalMapName.empty())
            stream << "map_bump ../images/" << material.normalMapName << ".dds\n";

        if (!material.specularMapName.empty())
            stream << "map_Ks ../images/" << material.specularMapName << ".dds\n";
    }
}
