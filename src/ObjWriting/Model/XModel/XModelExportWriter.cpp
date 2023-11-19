#include "XModelExportWriter.h"

#include "Math/Quaternion.h"

#include <iomanip>
#include <iostream>

class XModelExportWriterBase : public XModelExportWriter
{
protected:
    std::string m_game_name;
    std::string m_zone_name;

    VertexMerger m_vertex_merger;

    void PrepareVertexMerger()
    {
        m_vertex_merger = VertexMerger(m_vertices.size());

        auto vertexOffset = 0u;
        for (const auto& vertex : m_vertices)
        {
            XModelVertexBoneWeights weights{nullptr, 0};

            if (vertexOffset < m_vertex_bone_weights.size())
                weights = m_vertex_bone_weights[vertexOffset];

            m_vertex_merger.Add(VertexMergerPos{vertex.coordinates[0], vertex.coordinates[1], vertex.coordinates[2], weights.weights, weights.weightCount});

            vertexOffset++;
        }
    }

    void WriteHeader(std::ostream& stream, const int version) const
    {
        stream << "// OpenAssetTools XMODEL_EXPORT File\n";
        stream << "// Game Origin: " << m_game_name << "\n";
        stream << "// Zone Origin: " << m_zone_name << "\n";
        stream << "MODEL\n";
        stream << "VERSION " << version << "\n";
        stream << "\n";
    }

    void WriteBones(std::ostream& stream) const
    {
        stream << "NUMBONES " << m_bones.size() << "\n";
        size_t boneNum = 0u;
        for (const auto& bone : m_bones)
        {
            stream << "BONE " << boneNum << " ";
            if (bone.parentIndex < 0)
                stream << "-1";
            else
                stream << bone.parentIndex;

            stream << " \"" << bone.name << "\"\n";
            boneNum++;
        }
        stream << "\n";

        boneNum = 0u;
        for (const auto& bone : m_bones)
        {
            stream << "BONE " << boneNum << "\n";
            stream << "OFFSET ";
            stream << std::setprecision(6) << std::fixed << bone.globalOffset[0] << ", " << std::setprecision(6) << std::fixed << bone.globalOffset[1] << ", "
                   << std::setprecision(6) << std::fixed << bone.globalOffset[2] << "\n";

            stream << "SCALE ";
            stream << std::setprecision(6) << std::fixed << bone.scale[0] << ", " << std::setprecision(6) << std::fixed << bone.scale[1] << ", "
                   << std::setprecision(6) << std::fixed << bone.scale[2] << "\n";

            const Matrix32 mat = bone.globalRotation.ToMatrix();
            stream << "X " << std::setprecision(6) << std::fixed << mat.m_data[0][0] << ", " << std::setprecision(6) << std::fixed << mat.m_data[1][0] << ", "
                   << std::setprecision(6) << std::fixed << mat.m_data[2][0] << "\n";
            stream << "Y " << std::setprecision(6) << std::fixed << mat.m_data[0][1] << ", " << std::setprecision(6) << std::fixed << mat.m_data[1][1] << ", "
                   << std::setprecision(6) << std::fixed << mat.m_data[2][1] << "\n";
            stream << "Z " << std::setprecision(6) << std::fixed << mat.m_data[0][2] << ", " << std::setprecision(6) << std::fixed << mat.m_data[1][2] << ", "
                   << std::setprecision(6) << std::fixed << mat.m_data[2][2] << "\n";
            stream << "\n";
            boneNum++;
        }
    }

    XModelExportWriterBase(std::string gameName, std::string zoneName)
        : m_game_name(std::move(gameName)),
          m_zone_name(std::move(zoneName))
    {
    }
};

class XModelExportWriter6 final : public XModelExportWriterBase
{
    void WriteVertices(std::ostream& stream) const
    {
        const auto& distinctVertexValues = m_vertex_merger.GetDistinctValues();
        stream << "NUMVERTS " << distinctVertexValues.size() << "\n";
        size_t vertexNum = 0u;
        for (const auto& vertexPos : distinctVertexValues)
        {
            stream << "VERT " << vertexNum << "\n";
            stream << "OFFSET ";
            stream << std::setprecision(6) << std::fixed << vertexPos.x << ", " << std::setprecision(6) << std::fixed << vertexPos.y << ", "
                   << std::setprecision(6) << std::fixed << vertexPos.z << "\n";
            stream << "BONES " << vertexPos.weightCount << "\n";

            for (auto weightIndex = 0u; weightIndex < vertexPos.weightCount; weightIndex++)
            {
                stream << "BONE " << vertexPos.weights[weightIndex].boneIndex << " " << std::setprecision(6) << std::fixed
                       << vertexPos.weights[weightIndex].weight << "\n";
            }
            stream << "\n";
            vertexNum++;
        }
    }

    static void WriteFaceVertex(std::ostream& stream, const size_t index, const XModelVertex& vertex)
    {
        stream << "VERT " << index << "\n";
        stream << "NORMAL " << std::setprecision(6) << std::fixed << vertex.normal[0] << " " << std::setprecision(6) << std::fixed << vertex.normal[1] << " "
               << std::setprecision(6) << std::fixed << vertex.normal[2] << "\n";
        stream << "COLOR " << std::setprecision(6) << std::fixed << vertex.color[0] << " " << std::setprecision(6) << std::fixed << vertex.color[1] << " "
               << std::setprecision(6) << std::fixed << vertex.color[2] << " " << std::setprecision(6) << std::fixed << vertex.color[3] << "\n";
        stream << "UV 1 " << std::setprecision(6) << std::fixed << vertex.uv[0] << " " << std::setprecision(6) << std::fixed << vertex.uv[1] << "\n";
    }

    void WriteFaces(std::ostream& stream) const
    {
        stream << "NUMFACES " << m_faces.size() << "\n";
        for (const auto& face : m_faces)
        {
            const size_t distinctPositions[3]{
                m_vertex_merger.GetDistinctPositionByInputPosition(face.vertexIndex[0]),
                m_vertex_merger.GetDistinctPositionByInputPosition(face.vertexIndex[1]),
                m_vertex_merger.GetDistinctPositionByInputPosition(face.vertexIndex[2]),
            };

            const XModelVertex& v0 = m_vertices[face.vertexIndex[0]];
            const XModelVertex& v1 = m_vertices[face.vertexIndex[1]];
            const XModelVertex& v2 = m_vertices[face.vertexIndex[2]];

            stream << "TRI " << face.objectIndex << " " << face.materialIndex << " 0 0\n";
            WriteFaceVertex(stream, distinctPositions[0], v0);
            WriteFaceVertex(stream, distinctPositions[1], v1);
            WriteFaceVertex(stream, distinctPositions[2], v2);
            stream << "\n";
        }
    }

    void WriteObjects(std::ostream& stream) const
    {
        stream << "NUMOBJECTS " << m_objects.size() << "\n";
        size_t objectNum = 0u;
        for (const auto& object : m_objects)
        {
            stream << "OBJECT " << objectNum << " \"" << object.name << "\"\n";
            objectNum++;
        }
        stream << "\n";
    }

    void WriteMaterials(std::ostream& stream) const
    {
        stream << "NUMMATERIALS " << m_materials.size() << "\n";
        size_t materialNum = 0u;
        for (const auto& material : m_materials)
        {
            const auto colorMapPath = "../images/" + material.colorMapName + ".dds";
            stream << "MATERIAL " << materialNum << " \"" << material.name << "\" \"" << material.materialTypeName << "\" \"" << colorMapPath << "\"\n";
            stream << "COLOR " << std::setprecision(6) << std::fixed << material.color[0] << " " << std::setprecision(6) << std::fixed << material.color[1]
                   << " " << std::setprecision(6) << std::fixed << material.color[2] << " " << std::setprecision(6) << std::fixed << material.color[3] << "\n";
            stream << "TRANSPARENCY " << std::setprecision(6) << std::fixed << material.transparency[0] << " " << std::setprecision(6) << std::fixed
                   << material.transparency[1] << " " << std::setprecision(6) << std::fixed << material.transparency[2] << " " << std::setprecision(6)
                   << std::fixed << material.transparency[3] << "\n";
            stream << "AMBIENTCOLOR " << std::setprecision(6) << std::fixed << material.ambientColor[0] << " " << std::setprecision(6) << std::fixed
                   << material.ambientColor[1] << " " << std::setprecision(6) << std::fixed << material.ambientColor[2] << " " << std::setprecision(6)
                   << std::fixed << material.ambientColor[3] << "\n";
            stream << "INCANDESCENCE " << std::setprecision(6) << std::fixed << material.incandescence[0] << " " << std::setprecision(6) << std::fixed
                   << material.incandescence[1] << " " << std::setprecision(6) << std::fixed << material.incandescence[2] << " " << std::setprecision(6)
                   << std::fixed << material.incandescence[3] << "\n";
            stream << "COEFFS " << std::setprecision(6) << std::fixed << material.coeffs[0] << " " << std::setprecision(6) << std::fixed << material.coeffs[1]
                   << "\n";
            stream << "GLOW " << std::setprecision(6) << std::fixed << material.glow.x << " " << material.glow.y << "\n";
            stream << "REFRACTIVE " << material.refractive.x << " " << std::setprecision(6) << std::fixed << material.refractive.y << "\n";
            stream << "SPECULARCOLOR " << std::setprecision(6) << std::fixed << material.specularColor[0] << " " << std::setprecision(6) << std::fixed
                   << material.specularColor[1] << " " << std::setprecision(6) << std::fixed << material.specularColor[2] << " " << std::setprecision(6)
                   << std::fixed << material.specularColor[3] << "\n";
            stream << "REFLECTIVECOLOR " << std::setprecision(6) << std::fixed << material.reflectiveColor[0] << " " << std::setprecision(6) << std::fixed
                   << material.reflectiveColor[1] << " " << std::setprecision(6) << std::fixed << material.reflectiveColor[2] << " " << std::setprecision(6)
                   << std::fixed << material.reflectiveColor[3] << "\n";
            stream << "REFLECTIVE " << material.reflective.x << " " << std::setprecision(6) << std::fixed << material.reflective.y << "\n";
            stream << "BLINN " << std::setprecision(6) << std::fixed << material.blinn[0] << " " << std::setprecision(6) << std::fixed << material.blinn[1]
                   << "\n";
            stream << "PHONG " << std::setprecision(6) << std::fixed << material.phong << "\n";
            stream << "\n";
            materialNum++;
        }
    }

public:
    XModelExportWriter6(std::string gameName, std::string zoneName)
        : XModelExportWriterBase(std::move(gameName), std::move(zoneName))
    {
    }

    void Write(std::ostream& stream) override
    {
        PrepareVertexMerger();
        WriteHeader(stream, 6);
        WriteBones(stream);
        WriteVertices(stream);
        WriteFaces(stream);
        WriteObjects(stream);
        WriteMaterials(stream);
    }
};

std::unique_ptr<XModelExportWriter> XModelExportWriter::CreateWriterForVersion6(std::string gameName, std::string zoneName)
{
    return std::make_unique<XModelExportWriter6>(std::move(gameName), std::move(zoneName));
}
