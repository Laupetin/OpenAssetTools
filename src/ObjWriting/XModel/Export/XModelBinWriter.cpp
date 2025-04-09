#include "XModelBinWriter.h"

#include "Utils/MemoryWriter.h"

#pragma warning(push, 0)
#include <Eigen>
#pragma warning(pop)

#include <chrono>
#include <iomanip>
#include <iostream>
#include <lib/lz4.h>

class XModelBinWriterBase : public XModelWriter
{
protected:
    void PrepareVertexMerger(const XModelCommon& xmodel)
    {
        m_vertex_merger = VertexMerger(xmodel.m_vertices.size());

        auto vertexOffset = 0u;
        for (const auto& vertex : xmodel.m_vertices)
        {
            XModelVertexBoneWeights weights{0, 0};

            if (vertexOffset < xmodel.m_vertex_bone_weights.size())
                weights = xmodel.m_vertex_bone_weights[vertexOffset];

            m_vertex_merger.Add(VertexMergerPos{vertex.coordinates[0],
                                                vertex.coordinates[1],
                                                vertex.coordinates[2],
                                                xmodel.m_bone_weight_data.weights.empty() ? nullptr : &xmodel.m_bone_weight_data.weights[weights.weightOffset],
                                                weights.weightCount});

            vertexOffset++;
        }
    }

    void WriteAlignedString(const std::string& string)
    {
        auto paddingSize = ((string.size() + 1 + 0x3) & 0xFFFFFFFFFFFFFC) - (string.size() + 1);
        auto padding = std::make_unique<uint8_t[]>(paddingSize);

        m_writer.WriteNullTerminatedString(string);

        m_writer.Write(reinterpret_cast<uint8_t*>(padding.get()), paddingSize);
    }

    void WriteComment(const std::string& comment)
    {
        m_writer.Write(0xC355);
        WriteAlignedString(comment);
    }

    void WriteInt16(const int16_t hash, const int16_t value)
    {
        m_writer.Write(hash);
        m_writer.Write(value);
    }

    void WriteUInt16(const int16_t hash, const uint16_t value)
    {
        m_writer.Write(hash);
        m_writer.Write(value);
    }

    int16_t ClampFloatToShort(const float value) const
    {
        return std::clamp(static_cast<int16_t>(32767 * std::clamp(value, -1.0f, 1.0f)), static_cast<int16_t>(-32767), 32767i16);
    }

    uint8_t ClampFloatToUByte(const float value) const
    {
        return std::clamp(static_cast<uint8_t>(255 * std::clamp(value, -1.0f, 1.0f)), 0ui8, 255ui8);
    }

    void WriteHeader(int16_t version)
    {
        WriteComment("OpenAssetTools XMODEL_BIN File");
        WriteComment(std::format("Game Origin: {}", m_game_name));
        WriteComment(std::format("Zone Origin: {}", m_zone_name));
        m_writer.Write(0x46C8); // Model Hash
        WriteInt16(0x24D1, version);
    }

    void WriteBones(const XModelCommon& xmodel)
    {
        WriteInt16(0x76BA, xmodel.m_bones.size());

        auto boneNum = 0;
        for (const auto& bone : xmodel.m_bones)
        {
            m_writer.Write(0xF099); // Bone Hash
            m_writer.Write(boneNum);
            if (bone.parentIndex)
                m_writer.Write(static_cast<int32_t>(*bone.parentIndex));
            else
                m_writer.Write(-1);

            WriteAlignedString(bone.name);
            boneNum++;
        }

        boneNum = 0;
        for (const auto& bone : xmodel.m_bones)
        {
            WriteInt16(0xDD9A, boneNum);

            m_writer.Write(0x9383);               // Bone Offset Hash
            m_writer.Write(bone.globalOffset[0]); // X
            m_writer.Write(bone.globalOffset[1]); // Y
            m_writer.Write(bone.globalOffset[2]); // Z

            m_writer.Write(0x1C56);        // Bone Scale Hash
            m_writer.Write(bone.scale[0]); // X
            m_writer.Write(bone.scale[1]); // Y
            m_writer.Write(bone.scale[2]); // Z

            const auto mat = Eigen::Quaternionf(bone.globalRotation.w, bone.globalRotation.x, bone.globalRotation.y, bone.globalRotation.z).matrix();

            m_writer.Write(0xDCFDi16); // Bone Matrix X Hash
            m_writer.Write(ClampFloatToShort(mat(0, 0)));
            m_writer.Write(ClampFloatToShort(mat(0, 1)));
            m_writer.Write(ClampFloatToShort(mat(0, 2)));

            m_writer.Write(0xCCDCi16); // Bone Matrix Y Hash
            m_writer.Write(ClampFloatToShort(mat(1, 0)));
            m_writer.Write(ClampFloatToShort(mat(1, 1)));
            m_writer.Write(ClampFloatToShort(mat(1, 2)));

            m_writer.Write(0xFCBFi16); // Bone Matrix Z Hash
            m_writer.Write(ClampFloatToShort(mat(2, 0)));
            m_writer.Write(ClampFloatToShort(mat(2, 1)));
            m_writer.Write(ClampFloatToShort(mat(2, 2)));

            boneNum++;
        }
    }

    XModelBinWriterBase(std::ostream& stream, std::string gameName, std::string zoneName)
        : m_writer(0x800000), // 8MB
          m_stream(stream),
          m_game_name(std::move(gameName)),
          m_zone_name(std::move(zoneName))
    {
    }

    MemoryWriter m_writer;
    std::ostream& m_stream;
    std::string m_game_name;
    std::string m_zone_name;
    VertexMerger m_vertex_merger;
};

class XModelBinWriter7 final : public XModelBinWriterBase
{
    void WriteVertices(const XModelCommon& xmodel)
    {
        const auto& distinctVertexValues = m_vertex_merger.GetDistinctValues();

        if (distinctVertexValues.size() > UINT16_MAX)
        {
            // Use 32 bit
            m_writer.Write(0x2AEC); // 32 bit Hash
            m_writer.Write(static_cast<uint32_t>(distinctVertexValues.size()));
        }
        else
        {
            // Use 16 bit
            WriteUInt16(0x950D, distinctVertexValues.size());
        }

        size_t vertexNum = 0u;
        for (const auto& vertexPos : distinctVertexValues)
        {
            if (vertexNum > UINT16_MAX)
            {
                // Use 32 bit
                m_writer.Write(0xB097); // 32 bit Hash
                m_writer.Write(static_cast<uint32_t>(vertexNum));
            }
            else
            {
                // Use 16 bit
                WriteUInt16(0x8F03, vertexNum);
            }

            m_writer.Write(0x9383); // Offset Hash
            m_writer.Write(vertexPos.x);
            m_writer.Write(vertexPos.y);
            m_writer.Write(vertexPos.z);

            WriteInt16(0xEA46, vertexPos.weightCount);

            for (auto weightIndex = 0u; weightIndex < vertexPos.weightCount; weightIndex++)
            {
                const auto& weight = vertexPos.weights[weightIndex];

                WriteInt16(0xF1AB, weight.boneIndex);
                m_writer.Write(weight.weight);
            }
            vertexNum++;
        }
    }

    void WriteFaceVertex(const XModelVertex& vertex)
    {
        m_writer.Write(0x89ECi16);                           // Normal Hash
        m_writer.Write(ClampFloatToShort(vertex.normal[0])); // X
        m_writer.Write(ClampFloatToShort(vertex.normal[1])); // Y
        m_writer.Write(ClampFloatToShort(vertex.normal[2])); // Z

        m_writer.Write(0x6DD8);                             // Color Hash
        m_writer.Write(ClampFloatToUByte(vertex.color[0])); // R
        m_writer.Write(ClampFloatToUByte(vertex.color[1])); // G
        m_writer.Write(ClampFloatToUByte(vertex.color[2])); // B
        m_writer.Write(ClampFloatToUByte(vertex.color[3])); // A

        m_writer.Write(0x1AD4i16); // UV Hash
        m_writer.Write(1ui16);     // Layer
        m_writer.Write(vertex.uv[0]);
        m_writer.Write(vertex.uv[1]);
    }

    void WriteFaces(const XModelCommon& xmodel)
    {
        auto totalFaceCount = 0u;
        for (const auto& object : xmodel.m_objects)
            totalFaceCount += object.m_faces.size();

        m_writer.Write(0xBE92); // Face Count Hash
        m_writer.Write(totalFaceCount);

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

                m_writer.Write(0x562Fi16); // Face Info Hash
                m_writer.Write(static_cast<uint8_t>(objectIndex));
                m_writer.Write(static_cast<uint8_t>(object.materialIndex));

                if (m_vertex_merger.GetDistinctValues().size() > UINT16_MAX)
                {
                    m_writer.Write(0xB097); // 32 Bit Vertex Count Hash
                    m_writer.Write(static_cast<uint32_t>(distinctPositions[0]));
                    WriteFaceVertex(v0);

                    m_writer.Write(0xB097); // 32 Bit Vertex Count Hash
                    m_writer.Write(static_cast<uint32_t>(distinctPositions[1]));
                    WriteFaceVertex(v1);

                    m_writer.Write(0xB097); // 32 Bit Vertex Count Hash
                    m_writer.Write(static_cast<uint32_t>(distinctPositions[2]));
                    WriteFaceVertex(v2);
                }
                else
                {
                    WriteUInt16(0x8F03, distinctPositions[0]);
                    WriteFaceVertex(v0);

                    WriteUInt16(0x8F03, distinctPositions[1]);
                    WriteFaceVertex(v1);

                    WriteUInt16(0x8F03, distinctPositions[2]);
                    WriteFaceVertex(v2);
                }
            }

            objectIndex++;
        }
    }

    void WriteObjects(const XModelCommon& xmodel)
    {
        WriteInt16(0x62AF, xmodel.m_objects.size());

        size_t objectNum = 0;
        for (const auto& object : xmodel.m_objects)
        {
            m_writer.Write(0x87D4i16); // Object Info Hash
            m_writer.Write(static_cast<int16_t>(objectNum));
            WriteAlignedString(object.name);

            objectNum++;
        }
    }

    void WriteMaterials(const XModelCommon& xmodel)
    {
        WriteInt16(0xA1B2, xmodel.m_materials.size());

        size_t materialNum = 0u;
        for (const auto& material : xmodel.m_materials)
        {
            const auto colorMapPath = "../images/" + material.colorMapName + ".dds";

            WriteInt16(0xA700, materialNum);
            WriteAlignedString(material.name);
            WriteAlignedString(material.materialTypeName);
            WriteAlignedString(colorMapPath);

            m_writer.Write(0x6DD8);                               // Vertex Color Hash
            m_writer.Write(ClampFloatToUByte(material.color[0])); // R
            m_writer.Write(ClampFloatToUByte(material.color[1])); // G
            m_writer.Write(ClampFloatToUByte(material.color[2])); // B
            m_writer.Write(ClampFloatToUByte(material.color[3])); // A

            m_writer.Write(0x6DAB); // Transparancy Hash
            m_writer.Write(material.transparency[0]);
            m_writer.Write(material.transparency[1]);
            m_writer.Write(material.transparency[2]);
            m_writer.Write(material.transparency[3]);

            m_writer.Write(0x37FF);                   // Ambient Color Hash
            m_writer.Write(material.ambientColor[0]); // R
            m_writer.Write(material.ambientColor[1]); // G
            m_writer.Write(material.ambientColor[2]); // B
            m_writer.Write(material.ambientColor[3]); // A

            m_writer.Write(0x4265); // Incandescence Hash
            m_writer.Write(material.incandescence[0]);
            m_writer.Write(material.incandescence[1]);
            m_writer.Write(material.incandescence[2]);
            m_writer.Write(material.incandescence[3]);

            m_writer.Write(0xC835); // Coeffs Hash
            m_writer.Write(material.coeffs[0]);
            m_writer.Write(material.coeffs[1]);

            m_writer.Write(0xFE0C); // Glow Hash
            m_writer.Write(material.glow.x);
            m_writer.Write(material.glow.y);

            m_writer.Write(0x7E24); // Refractive Hash
            m_writer.Write(material.refractive.x);
            m_writer.Write(material.refractive.y);

            m_writer.Write(0x317C);                    // Specular Color Hash
            m_writer.Write(material.specularColor[0]); // R
            m_writer.Write(material.specularColor[1]); // G
            m_writer.Write(material.specularColor[2]); // B
            m_writer.Write(material.specularColor[3]); // A

            m_writer.Write(0xE593);                      // Reflective Color Hash
            m_writer.Write(material.reflectiveColor[0]); // R
            m_writer.Write(material.reflectiveColor[1]); // G
            m_writer.Write(material.reflectiveColor[2]); // B
            m_writer.Write(material.reflectiveColor[3]); // A

            m_writer.Write(0x7D76); // Reflective Hash
            m_writer.Write(material.reflective.x);
            m_writer.Write(material.reflective.y);

            m_writer.Write(0x83C7); // Blinn Hash
            m_writer.Write(material.blinn[0]);
            m_writer.Write(material.blinn[1]);

            m_writer.Write(0x5CD2); // Phong Hash
            m_writer.Write(material.phong);

            materialNum++;
        }
    }

public:
    XModelBinWriter7(std::ostream& stream, std::string gameName, std::string zoneName)
        : XModelBinWriterBase(stream, std::move(gameName), std::move(zoneName))
    {
    }

    void Write(const XModelCommon& xmodel) override
    {
        PrepareVertexMerger(xmodel);
        WriteHeader(7);
        WriteBones(xmodel);
        WriteVertices(xmodel);
        WriteFaces(xmodel);
        WriteObjects(xmodel);
        WriteMaterials(xmodel);

        auto estimatedCompressedFileSize = LZ4_compressBound(m_writer.GetPosition());
        auto compressedBuffer = new char[estimatedCompressedFileSize];
        auto actualCompressedFileSize =
            LZ4_compress_default(reinterpret_cast<const char*>(m_writer.Data()), compressedBuffer, m_writer.GetPosition(), estimatedCompressedFileSize);

        uint64_t uncompressedSize = m_writer.GetPosition();
        char uncompressedSizeChar[4];
        std::memcpy(uncompressedSizeChar, &uncompressedSize, 4);

        const char magic[5] = {0x2A, 0x4C, 0x5A, 0x34, 0x2A};
        m_stream.write(magic, 5);
        m_stream.write(uncompressedSizeChar, 4);
        m_stream.write(compressedBuffer, actualCompressedFileSize);
    }
};

namespace xmodel_bin
{
    std::unique_ptr<XModelWriter> CreateWriterForVersion7(std::ostream& stream, std::string gameName, std::string zoneName)
    {
        return std::make_unique<XModelBinWriter7>(stream, std::move(gameName), std::move(zoneName));
    }
} // namespace xmodel_bin
