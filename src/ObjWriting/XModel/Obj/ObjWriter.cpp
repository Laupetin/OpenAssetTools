#include "ObjWriter.h"

#include "Utils/DistinctMapper.h"
#include "XModel/Obj/ObjCommon.h"

namespace
{
    struct ObjObjectData
    {
        DistinctMapper<ObjVertex> m_vertices;
        DistinctMapper<ObjNormal> m_normals;
        DistinctMapper<ObjUv> m_uvs;
    };

    struct ObjObjectDataOffsets
    {
        size_t vertexOffset;
        size_t normalOffset;
        size_t uvOffset;
    };

    class ObjWriter final : public XModelWriter
    {
    public:
        ObjWriter(std::ostream& stream, std::string gameName, std::string zoneName, std::string mtlName)
            : m_stream(stream),
              m_mtl_name(std::move(mtlName)),
              m_game_name(std::move(gameName)),
              m_zone_name(std::move(zoneName))
        {
        }

        void Write(const XModelCommon& xmodel) override
        {
            m_stream << "# OpenAssetTools OBJ File ( " << m_game_name << ")\n";
            m_stream << "# Game Origin: " << m_game_name << "\n";
            m_stream << "# Zone Origin: " << m_zone_name << "\n";

            if (!m_mtl_name.empty())
                m_stream << "mtllib " << m_mtl_name << "\n";

            std::vector<ObjObjectDataOffsets> inputOffsetsByObject;
            std::vector<ObjObjectDataOffsets> distinctOffsetsByObject;
            GetObjObjectDataOffsets(xmodel, inputOffsetsByObject, distinctOffsetsByObject);

            auto objectIndex = 0;
            for (const auto& object : xmodel.m_objects)
            {
                const auto& objectData = m_object_data[objectIndex];
                m_stream << "o " << object.name << "\n";

                for (const auto& v : objectData.m_vertices.GetDistinctValues())
                    m_stream << "v " << v.coordinates[0] << " " << v.coordinates[1] << " " << v.coordinates[2] << "\n";
                for (const auto& uv : objectData.m_uvs.GetDistinctValues())
                    m_stream << "vt " << uv.uv[0] << " " << uv.uv[1] << "\n";
                for (const auto& n : objectData.m_normals.GetDistinctValues())
                    m_stream << "vn " << n.normal[0] << " " << n.normal[1] << " " << n.normal[2] << "\n";

                if (object.materialIndex >= 0 && static_cast<unsigned>(object.materialIndex) < xmodel.m_materials.size())
                    m_stream << "usemtl " << xmodel.m_materials[object.materialIndex].name << "\n";

                auto faceIndex = 0u;
                for (const auto& f : object.m_faces)
                {
                    const auto faceVertexOffset = 3u * faceIndex;
                    const size_t v[3]{
                        objectData.m_vertices.GetDistinctPositionByInputPosition(faceVertexOffset + 0) + distinctOffsetsByObject[objectIndex].vertexOffset + 1,
                        objectData.m_vertices.GetDistinctPositionByInputPosition(faceVertexOffset + 1) + distinctOffsetsByObject[objectIndex].vertexOffset + 1,
                        objectData.m_vertices.GetDistinctPositionByInputPosition(faceVertexOffset + 2) + distinctOffsetsByObject[objectIndex].vertexOffset + 1,
                    };
                    const size_t n[3]{
                        objectData.m_normals.GetDistinctPositionByInputPosition(faceVertexOffset + 0) + distinctOffsetsByObject[objectIndex].normalOffset + 1,
                        objectData.m_normals.GetDistinctPositionByInputPosition(faceVertexOffset + 1) + distinctOffsetsByObject[objectIndex].normalOffset + 1,
                        objectData.m_normals.GetDistinctPositionByInputPosition(faceVertexOffset + 2) + distinctOffsetsByObject[objectIndex].normalOffset + 1,
                    };
                    const size_t uv[3]{
                        objectData.m_uvs.GetDistinctPositionByInputPosition(faceVertexOffset + 0) + distinctOffsetsByObject[objectIndex].uvOffset + 1,
                        objectData.m_uvs.GetDistinctPositionByInputPosition(faceVertexOffset + 1) + distinctOffsetsByObject[objectIndex].uvOffset + 1,
                        objectData.m_uvs.GetDistinctPositionByInputPosition(faceVertexOffset + 2) + distinctOffsetsByObject[objectIndex].uvOffset + 1,
                    };

                    m_stream << std::format("f {}/{}/{} {}/{}/{} {}/{}/{}\n", v[0], uv[0], n[0], v[1], uv[1], n[1], v[2], uv[2], n[2]);
                    faceIndex++;
                }

                objectIndex++;
            }
        }

        void GetObjObjectDataOffsets(const XModelCommon& xmodel,
                                     std::vector<ObjObjectDataOffsets>& inputOffsets,
                                     std::vector<ObjObjectDataOffsets>& distinctOffsets)
        {
            ObjObjectDataOffsets currentInputOffsets{};
            ObjObjectDataOffsets currentDistinctOffsets{};

            AddObjFaces(xmodel);

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

        void AddObjFaces(const XModelCommon& common)
        {
            m_object_data.resize(common.m_objects.size());

            auto objectIndex = 0u;
            for (const auto& commonObject : common.m_objects)
            {
                auto& objectData = m_object_data[objectIndex];

                for (const auto& commonFace : commonObject.m_faces)
                {
                    ObjFace face{};
                    face.vertexIndex[0] = commonFace.vertexIndex[2];
                    face.vertexIndex[1] = commonFace.vertexIndex[1];
                    face.vertexIndex[2] = commonFace.vertexIndex[0];
                    face.normalIndex[0] = face.vertexIndex[0];
                    face.normalIndex[1] = face.vertexIndex[1];
                    face.normalIndex[2] = face.vertexIndex[2];
                    face.uvIndex[0] = face.vertexIndex[0];
                    face.uvIndex[1] = face.vertexIndex[1];
                    face.uvIndex[2] = face.vertexIndex[2];

                    // Reverse order for OBJ
                    AddObjVertex(objectData, common, commonFace.vertexIndex[2]);
                    AddObjVertex(objectData, common, commonFace.vertexIndex[1]);
                    AddObjVertex(objectData, common, commonFace.vertexIndex[0]);
                }

                objectIndex++;
            }
        }

        static void AddObjVertex(ObjObjectData& objectData, const XModelCommon& common, const int commonVertexIndex)
        {
            const auto& commonVertex = common.m_vertices[commonVertexIndex];

            ObjVertex objVertex{};
            objVertex.coordinates[0] = commonVertex.coordinates[0];
            objVertex.coordinates[1] = commonVertex.coordinates[2];
            objVertex.coordinates[2] = -commonVertex.coordinates[1];
            objectData.m_vertices.Add(objVertex);

            ObjNormal objNormal{};
            objNormal.normal[0] = commonVertex.normal[0];
            objNormal.normal[1] = commonVertex.normal[2];
            objNormal.normal[2] = -commonVertex.normal[1];
            objectData.m_normals.Add(objNormal);

            ObjUv objUv{};
            objUv.uv[0] = commonVertex.uv[0];
            objUv.uv[1] = 1.0f - commonVertex.uv[1];
            objectData.m_uvs.Add(objUv);
        }

        std::ostream& m_stream;
        std::string m_mtl_name;
        std::string m_game_name;
        std::string m_zone_name;
        std::vector<ObjObjectData> m_object_data;
    };

    class MtlWriter final : public XModelWriter
    {
    public:
        MtlWriter(std::ostream& stream, std::string gameName, std::string zoneName)
            : m_stream(stream),
              m_game_name(std::move(gameName)),
              m_zone_name(std::move(zoneName))
        {
        }

        void Write(const XModelCommon& xmodel) override
        {
            m_stream << "# OpenAssetTools MAT File ( " << m_game_name << ")\n";
            m_stream << "# Game Origin: " << m_game_name << "\n";
            m_stream << "# Zone Origin: " << m_zone_name << "\n";
            m_stream << "# Material count: " << xmodel.m_materials.size() << "\n";

            for (const auto& material : xmodel.m_materials)
            {
                m_stream << "\n";
                m_stream << "newmtl " << material.name << "\n";

                if (!material.colorMapName.empty())
                    m_stream << "map_Kd ../images/" << material.colorMapName << ".dds\n";

                if (!material.normalMapName.empty())
                    m_stream << "map_bump ../images/" << material.normalMapName << ".dds\n";

                if (!material.specularMapName.empty())
                    m_stream << "map_Ks ../images/" << material.specularMapName << ".dds\n";
            }
        }

    private:
        std::ostream& m_stream;
        std::string m_game_name;
        std::string m_zone_name;
    };
} // namespace

namespace obj
{
    std::unique_ptr<XModelWriter> CreateObjWriter(std::ostream& stream, std::string mtlName, std::string gameName, std::string zoneName)
    {
        return std::make_unique<ObjWriter>(stream, std::move(mtlName), std::move(gameName), std::move(zoneName));
    }

    std::unique_ptr<XModelWriter> CreateMtlWriter(std::ostream& stream, std::string gameName, std::string zoneName)
    {
        return std::make_unique<MtlWriter>(stream, std::move(gameName), std::move(zoneName));
    }
} // namespace obj
