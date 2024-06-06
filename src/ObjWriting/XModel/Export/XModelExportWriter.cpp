#include "XModelExportWriter.h"

#pragma warning(push, 0)
#include <Eigen>
#pragma warning(pop)

#include <chrono>
#include <iomanip>
#include <iostream>

class XModelExportWriterBase : public XModelWriter
{
protected:
    void PrepareVertexMerger(const XModelCommon& xmodel)
    {
        m_vertex_merger = VertexMerger(xmodel.m_vertices.size());

        auto vertexOffset = 0u;
        for (const auto& vertex : xmodel.m_vertices)
        {
            XModelVertexBoneWeights weights{nullptr, 0};

            if (vertexOffset < xmodel.m_vertex_bone_weights.size())
                weights = xmodel.m_vertex_bone_weights[vertexOffset];

            m_vertex_merger.Add(VertexMergerPos{vertex.coordinates[0], vertex.coordinates[1], vertex.coordinates[2], weights.weights, weights.weightCount});

            vertexOffset++;
        }
    }

    void WriteHeader(const int version) const
    {
        m_stream << "// OpenAssetTools XMODEL_EXPORT File\n";
        m_stream << "// Game Origin: " << m_game_name << "\n";
        m_stream << "// Zone Origin: " << m_zone_name << "\n";
        m_stream << "MODEL\n";
        m_stream << "VERSION " << version << "\n";
        m_stream << "\n";
    }

    void WriteBones(const XModelCommon& xmodel) const
    {
        m_stream << "NUMBONES " << xmodel.m_bones.size() << "\n";
        size_t boneNum = 0u;
        for (const auto& bone : xmodel.m_bones)
        {
            m_stream << "BONE " << boneNum << " ";
            if (bone.parentIndex)
                m_stream << *bone.parentIndex;
            else
                m_stream << "-1";

            m_stream << " \"" << bone.name << "\"\n";
            boneNum++;
        }
        m_stream << "\n";

        boneNum = 0u;
        for (const auto& bone : xmodel.m_bones)
        {
            m_stream << "BONE " << boneNum << "\n";
            m_stream << std::format("OFFSET {:.6f}, {:.6f}, {:.6f}\n", bone.globalOffset[0], bone.globalOffset[1], bone.globalOffset[2]);
            m_stream << std::format("SCALE {:.6f}, {:.6f}, {:.6f}\n", bone.scale[0], bone.scale[1], bone.scale[2]);

            const auto mat = Eigen::Quaternionf(bone.globalRotation.w, bone.globalRotation.x, bone.globalRotation.y, bone.globalRotation.z).matrix();
            m_stream << std::format("X {:.6f}, {:.6f}, {:.6f}\n", mat(0, 0), mat(1, 0), mat(2, 0));
            m_stream << std::format("Y {:.6f}, {:.6f}, {:.6f}\n", mat(0, 1), mat(1, 1), mat(2, 1));
            m_stream << std::format("Z {:.6f}, {:.6f}, {:.6f}\n", mat(0, 2), mat(1, 2), mat(2, 2));
            m_stream << '\n';
            boneNum++;
        }
    }

    XModelExportWriterBase(std::ostream& stream, std::string gameName, std::string zoneName)
        : m_stream(stream),
          m_game_name(std::move(gameName)),
          m_zone_name(std::move(zoneName))
    {
    }

    std::ostream& m_stream;
    std::string m_game_name;
    std::string m_zone_name;
    VertexMerger m_vertex_merger;
};

class XModelExportWriter6 final : public XModelExportWriterBase
{
    void WriteVertices(const XModelCommon& xmodel) const
    {
        const auto& distinctVertexValues = m_vertex_merger.GetDistinctValues();
        m_stream << "NUMVERTS " << distinctVertexValues.size() << "\n";
        size_t vertexNum = 0u;
        for (const auto& vertexPos : distinctVertexValues)
        {
            m_stream << "VERT " << vertexNum << "\n";
            m_stream << std::format("OFFSET {:.6f}, {:.6f}, {:.6f}\n", vertexPos.x, vertexPos.y, vertexPos.z);
            m_stream << "BONES " << vertexPos.weightCount << "\n";

            for (auto weightIndex = 0u; weightIndex < vertexPos.weightCount; weightIndex++)
            {
                const auto& weight = vertexPos.weights[weightIndex];
                m_stream << std::format("BONE {} {:.6f}\n", weight.boneIndex, weight.weight);
            }
            m_stream << "\n";
            vertexNum++;
        }
    }

    void WriteFaceVertex(const size_t index, const XModelVertex& vertex) const
    {
        m_stream << "VERT " << index << "\n";
        m_stream << std::format("NORMAL {:.6f} {:.6f} {:.6f}\n", vertex.normal[0], vertex.normal[1], vertex.normal[2]);
        m_stream << std::format("COLOR {:.6f} {:.6f} {:.6f} {:.6f}\n", vertex.color[0], vertex.color[1], vertex.color[2], vertex.color[3]);
        m_stream << std::format("UV 1 {:.6f} {:.6f}\n", vertex.uv[0], vertex.uv[1]);
    }

    void WriteFaces(const XModelCommon& xmodel) const
    {
        auto totalFaceCount = 0u;
        for (const auto& object : xmodel.m_objects)
            totalFaceCount += object.m_faces.size();

        m_stream << "NUMFACES " << totalFaceCount << "\n";

        auto objectIndex = 0u;
        for (const auto& object : xmodel.m_objects)
        {
            for (const auto& face : object.m_faces)
            {
                const size_t distinctPositions[3]{
                    m_vertex_merger.GetDistinctPositionByInputPosition(face.vertexIndex[0]),
                    m_vertex_merger.GetDistinctPositionByInputPosition(face.vertexIndex[1]),
                    m_vertex_merger.GetDistinctPositionByInputPosition(face.vertexIndex[2]),
                };

                const XModelVertex& v0 = xmodel.m_vertices[face.vertexIndex[0]];
                const XModelVertex& v1 = xmodel.m_vertices[face.vertexIndex[1]];
                const XModelVertex& v2 = xmodel.m_vertices[face.vertexIndex[2]];

                m_stream << "TRI " << objectIndex << " " << object.materialIndex << " 0 0\n";
                WriteFaceVertex(distinctPositions[0], v0);
                WriteFaceVertex(distinctPositions[1], v1);
                WriteFaceVertex(distinctPositions[2], v2);
                m_stream << "\n";
            }

            objectIndex++;
        }
    }

    void WriteObjects(const XModelCommon& xmodel) const
    {
        m_stream << "NUMOBJECTS " << xmodel.m_objects.size() << "\n";
        size_t objectNum = 0u;
        for (const auto& object : xmodel.m_objects)
        {
            m_stream << "OBJECT " << objectNum << " \"" << object.name << "\"\n";
            objectNum++;
        }
        m_stream << "\n";
    }

    void WriteMaterials(const XModelCommon& xmodel) const
    {
        m_stream << "NUMMATERIALS " << xmodel.m_materials.size() << "\n";
        size_t materialNum = 0u;
        for (const auto& material : xmodel.m_materials)
        {
            const auto colorMapPath = "../images/" + material.colorMapName + ".dds";
            m_stream << "MATERIAL " << materialNum << " \"" << material.name << "\" \"" << material.materialTypeName << "\" \"" << colorMapPath << "\"\n";
            m_stream << std::format("COLOR {:.6f} {:.6f} {:.6f} {:.6f}\n", material.color[0], material.color[1], material.color[2], material.color[3]);
            m_stream << std::format("TRANSPARENCY {:.6f} {:.6f} {:.6f} {:.6f}\n",
                                    material.transparency[0],
                                    material.transparency[1],
                                    material.transparency[2],
                                    material.transparency[3]);
            m_stream << std::format("AMBIENTCOLOR {:.6f} {:.6f} {:.6f} {:.6f}\n",
                                    material.ambientColor[0],
                                    material.ambientColor[1],
                                    material.ambientColor[2],
                                    material.ambientColor[3]);
            m_stream << std::format("INCANDESCENCE {:.6f} {:.6f} {:.6f} {:.6f}\n",
                                    material.incandescence[0],
                                    material.incandescence[1],
                                    material.incandescence[2],
                                    material.incandescence[3]);
            m_stream << std::format("COEFFS {:.6f} {:.6f}\n", material.coeffs[0], material.coeffs[1]);
            m_stream << std::format("GLOW {:.6f} {}\n", material.glow.x, material.glow.y);
            m_stream << std::format("REFRACTIVE {} {:.6f}\n", material.refractive.x, material.refractive.y);
            m_stream << std::format("SPECULARCOLOR {:.6f} {:.6f} {:.6f} {:.6f}\n",
                                    material.specularColor[0],
                                    material.specularColor[1],
                                    material.specularColor[2],
                                    material.specularColor[3]);
            m_stream << std::format("REFLECTIVECOLOR {:.6f} {:.6f} {:.6f} {:.6f}\n",
                                    material.reflectiveColor[0],
                                    material.reflectiveColor[1],
                                    material.reflectiveColor[2],
                                    material.reflectiveColor[3]);
            m_stream << std::format("REFLECTIVE {} {:.6f}\n", material.reflective.x, material.reflective.y);
            m_stream << std::format("BLINN {:.6f} {:.6f}\n", material.blinn[0], material.blinn[1]);
            m_stream << std::format("PHONG {:.6f}\n", material.phong);
            m_stream << '\n';
            materialNum++;
        }
    }

public:
    XModelExportWriter6(std::ostream& stream, std::string gameName, std::string zoneName)
        : XModelExportWriterBase(stream, std::move(gameName), std::move(zoneName))
    {
    }

    void Write(const XModelCommon& xmodel) override
    {
        PrepareVertexMerger(xmodel);
        WriteHeader(6);
        WriteBones(xmodel);
        WriteVertices(xmodel);
        WriteFaces(xmodel);
        WriteObjects(xmodel);
        WriteMaterials(xmodel);
    }
};

namespace xmodel_export
{
    std::unique_ptr<XModelWriter> CreateWriterForVersion6(std::ostream& stream, std::string gameName, std::string zoneName)
    {
        return std::make_unique<XModelExportWriter6>(stream, std::move(gameName), std::move(zoneName));
    }
} // namespace xmodel_export
