#include "XModelBinWriter.h"

#include "GitVersion.h"

#pragma warning(push, 0)
#include <Eigen>
#pragma warning(pop)

#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <lib/lz4.h>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>

class XModelBinWriterBase : public XModelWriter
{
protected:
    /*
     * A XModelBin Files consists of blocks of data, each block is indentified by a hash and is then followed by the actual data
     * e.g.:
     *       0x62AF      = Number of objects hash
     *       2           = Number of objects
     *       0x87D4      = Start of object hash
     *       0           = Object index
     *       Test_Obj    = Object name
     *       0x87D4      = Start of object hash
     *       1           = Object index
     *       Test_Obj2   = Object name
     */
    enum XModelBinHash : uint16_t
    {
        COMMENT = 0xC355,
        MODEL = 0x46C8,
        VERSION = 0x24D1,

        BONE_COUNT = 0x76BA,
        BONE = 0xF099,
        BONE_INDEX = 0xDD9A,
        OFFSET = 0x9383, // Bone/Vertex Offset
        BONE_SCALE = 0x1C56,
        BONE_MATRIX_X = 0xDCFD,
        BONE_MATRIX_Y = 0xCCDC,
        BONE_MATRIX_Z = 0xFCBF,

        VERT16_COUNT = 0x950D,
        VERT32_COUNT = 0x2AEC,
        VERT16 = 0x8F03,
        VERT32 = 0xB097,
        VERT_WEIGHT_COUNT = 0xEA46,
        VERT_WEIGHT = 0xF1AB,

        FACE_COUNT = 0xBE92,
        TRIANGLE16 = 0x6711,
        TRIANGLE32 = 0x562F,

        NORMAL = 0x89EC,
        COLOR = 0x6DD8,
        UV = 0x1AD4,

        OBJECT_COUNT = 0x62AF,
        OBJECT = 0x87D4,

        MATERIAL_COUNT = 0xA1B2,
        MATERIAL = 0xA700,
        MATERIAL_TRANSPARENCY = 0x6DAB,
        MATERIAL_AMBIENT_COLOR = 0x37FF,
        MATERIAL_INCANDESCENCE = 0x4265,
        MATERIAL_COEFFS = 0xC835,
        MATERIAL_GLOW = 0xFE0C,
        MATERIAL_REFRACTIVE = 0x7E24,
        MATERIAL_SPECULAR_COLOR = 0x317C,
        MATERIAL_REFLECTIVE_COLOR = 0xE593,
        MATERIAL_REFLECTIVE = 0x7D76,
        MATERIAL_BLINN = 0x83C7,
        MATERIAL_PHONG = 0x5CD2
    };

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

    template<typename T> void Write(const T& data)
    {
        m_writer.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    void WriteNullTerminatedString(std::string string)
    {
        m_writer.write(string.c_str(), string.size() + 1);
    }

    void WriteAlignedString(const std::string& string)
    {
        static constexpr uint8_t PADDING[4] = {0u, 0u, 0u, 0u};
        const auto paddingSize = (4u - (string.size() + 1u) % 4u) % 4u;

        WriteNullTerminatedString(string);
        m_writer.write(reinterpret_cast<const char*>(PADDING), paddingSize);
    }

    void WriteComment(const std::string& comment)
    {
        Write(static_cast<uint32_t>(XModelBinHash::COMMENT));
        WriteAlignedString(comment);
    }

    void WriteInt16(const uint16_t hash, const int16_t value)
    {
        Write(hash);
        Write(value);
    }

    void WriteUInt16(const uint16_t hash, const uint16_t value)
    {
        Write(hash);
        Write(value);
    }

    int16_t ClampFloatToShort(const float value) const
    {
        return std::clamp(static_cast<int16_t>(std::numeric_limits<int16_t>::max() * std::clamp(value, -1.0f, 1.0f)),
                          std::numeric_limits<int16_t>::min(),
                          std::numeric_limits<int16_t>::max());
    }

    uint8_t ClampFloatToUByte(const float value) const
    {
        return std::clamp(static_cast<uint8_t>(std::numeric_limits<uint8_t>::max() * std::clamp(value, -1.0f, 1.0f)),
                          std::numeric_limits<uint8_t>::min(),
                          std::numeric_limits<uint8_t>::max());
    }

    void WriteHeader(uint16_t version)
    {
        WriteComment("OpenAssetTools " GIT_VERSION " XMODEL_BIN File");
        WriteComment(std::format("Game Origin: {}", m_game_name));
        WriteComment(std::format("Zone Origin: {}", m_zone_name));
        Write(static_cast<uint32_t>(XModelBinHash::MODEL));
        WriteUInt16(XModelBinHash::VERSION, version);
    }

    void WriteBones(const XModelCommon& xmodel)
    {
        WriteUInt16(XModelBinHash::BONE_COUNT, static_cast<uint16_t>(xmodel.m_bones.size()));

        auto boneNum = 0U;
        for (const auto& bone : xmodel.m_bones)
        {
            Write(static_cast<uint32_t>(XModelBinHash::BONE));
            Write(boneNum);
            if (bone.parentIndex)
                Write(static_cast<int32_t>(*bone.parentIndex));
            else
                Write(-1);

            WriteAlignedString(bone.name);
            boneNum++;
        }

        boneNum = 0;
        for (const auto& bone : xmodel.m_bones)
        {
            WriteUInt16(XModelBinHash::BONE_INDEX, static_cast<uint32_t>(boneNum));

            Write(static_cast<uint32_t>(XModelBinHash::OFFSET));
            Write(bone.globalOffset[0]); // X
            Write(bone.globalOffset[1]); // Y
            Write(bone.globalOffset[2]); // Z

            Write(static_cast<uint32_t>(XModelBinHash::BONE_SCALE));
            Write(bone.scale[0]); // X
            Write(bone.scale[1]); // Y
            Write(bone.scale[2]); // Z

            const auto mat = Eigen::Quaternionf(bone.globalRotation.w, bone.globalRotation.x, bone.globalRotation.y, bone.globalRotation.z).matrix();

            Write(XModelBinHash::BONE_MATRIX_X);
            Write(ClampFloatToShort(mat(0, 0)));
            Write(ClampFloatToShort(mat(0, 1)));
            Write(ClampFloatToShort(mat(0, 2)));

            Write(XModelBinHash::BONE_MATRIX_Y);
            Write(ClampFloatToShort(mat(1, 0)));
            Write(ClampFloatToShort(mat(1, 1)));
            Write(ClampFloatToShort(mat(1, 2)));

            Write(XModelBinHash::BONE_MATRIX_Z);
            Write(ClampFloatToShort(mat(2, 0)));
            Write(ClampFloatToShort(mat(2, 1)));
            Write(ClampFloatToShort(mat(2, 2)));

            boneNum++;
        }
    }

    XModelBinWriterBase(std::string gameName, std::string zoneName)
        : m_game_name(std::move(gameName)),
          m_zone_name(std::move(zoneName))
    {
    }

    std::ostringstream m_writer;
    std::string m_game_name;
    std::string m_zone_name;
    VertexMerger m_vertex_merger;
};

class XModelBinWriter7 final : public XModelBinWriterBase
{
    void WriteVertices(const XModelCommon& xmodel)
    {
        const auto& distinctVertexValues = m_vertex_merger.GetDistinctValues();

        if (distinctVertexValues.size() > std::numeric_limits<uint16_t>::max())
        {
            // Use 32 bit
            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::VERT32_COUNT));
            XModelBinWriterBase::Write(static_cast<uint32_t>(distinctVertexValues.size()));
        }
        else
        {
            // Use 16 bit
            WriteUInt16(XModelBinHash::VERT16_COUNT, static_cast<uint16_t>(distinctVertexValues.size()));
        }

        size_t vertexNum = 0u;
        for (const auto& vertexPos : distinctVertexValues)
        {
            if (vertexNum > std::numeric_limits<uint16_t>::max())
            {
                // Use 32 bit
                XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::VERT32));
                XModelBinWriterBase::Write(static_cast<uint32_t>(vertexNum));
            }
            else
            {
                // Use 16 bit
                WriteUInt16(XModelBinHash::VERT16, static_cast<uint16_t>(vertexNum));
            }

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::OFFSET));
            XModelBinWriterBase::Write(vertexPos.x);
            XModelBinWriterBase::Write(vertexPos.y);
            XModelBinWriterBase::Write(vertexPos.z);

            WriteUInt16(XModelBinHash::VERT_WEIGHT_COUNT, static_cast<uint16_t>(vertexPos.weightCount));

            for (auto weightIndex = 0u; weightIndex < vertexPos.weightCount; weightIndex++)
            {
                const auto& weight = vertexPos.weights[weightIndex];

                WriteInt16(XModelBinHash::VERT_WEIGHT, weight.boneIndex);
                XModelBinWriterBase::Write(weight.weight);
            }
            vertexNum++;
        }
    }

    void WriteFaceVertex(const XModelVertex& vertex)
    {
        XModelBinWriterBase::Write(XModelBinHash::NORMAL);
        XModelBinWriterBase::Write(ClampFloatToShort(vertex.normal[0])); // X
        XModelBinWriterBase::Write(ClampFloatToShort(vertex.normal[1])); // Y
        XModelBinWriterBase::Write(ClampFloatToShort(vertex.normal[2])); // Z

        XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::COLOR));
        XModelBinWriterBase::Write(ClampFloatToUByte(vertex.color[0])); // R
        XModelBinWriterBase::Write(ClampFloatToUByte(vertex.color[1])); // G
        XModelBinWriterBase::Write(ClampFloatToUByte(vertex.color[2])); // B
        XModelBinWriterBase::Write(ClampFloatToUByte(vertex.color[3])); // A

        XModelBinWriterBase::Write(XModelBinHash::UV);
        XModelBinWriterBase::Write(static_cast<uint16_t>(1)); // Layer
        XModelBinWriterBase::Write(vertex.uv[0]);
        XModelBinWriterBase::Write(vertex.uv[1]);
    }

    void WriteFaces(const XModelCommon& xmodel)
    {
        auto totalFaceCount = 0u;
        for (const auto& object : xmodel.m_objects)
            totalFaceCount += object.m_faces.size();

        XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::FACE_COUNT));
        XModelBinWriterBase::Write(totalFaceCount);

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

                XModelBinWriterBase::Write(XModelBinHash::TRIANGLE32);
                XModelBinWriterBase::Write(static_cast<uint8_t>(objectIndex));
                XModelBinWriterBase::Write(static_cast<uint8_t>(object.materialIndex));

                if (m_vertex_merger.GetDistinctValues().size() > std::numeric_limits<uint16_t>::max())
                {
                    XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::VERT32));
                    XModelBinWriterBase::Write(static_cast<uint32_t>(distinctPositions[0]));
                    WriteFaceVertex(v0);

                    XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::VERT32));
                    XModelBinWriterBase::Write(static_cast<uint32_t>(distinctPositions[1]));
                    WriteFaceVertex(v1);

                    XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::VERT32));
                    XModelBinWriterBase::Write(static_cast<uint32_t>(distinctPositions[2]));
                    WriteFaceVertex(v2);
                }
                else
                {
                    WriteUInt16(XModelBinHash::VERT16, static_cast<uint16_t>(distinctPositions[0]));
                    WriteFaceVertex(v0);

                    WriteUInt16(XModelBinHash::VERT16, static_cast<uint16_t>(distinctPositions[1]));
                    WriteFaceVertex(v1);

                    WriteUInt16(XModelBinHash::VERT16, static_cast<uint16_t>(distinctPositions[2]));
                    WriteFaceVertex(v2);
                }
            }

            objectIndex++;
        }
    }

    void WriteObjects(const XModelCommon& xmodel)
    {
        WriteUInt16(XModelBinHash::OBJECT_COUNT, static_cast<uint16_t>(xmodel.m_objects.size()));

        size_t objectNum = 0;
        for (const auto& object : xmodel.m_objects)
        {
            XModelBinWriterBase::Write(XModelBinHash::OBJECT);
            XModelBinWriterBase::Write(static_cast<uint16_t>(objectNum));
            WriteAlignedString(object.name);

            objectNum++;
        }
    }

    void WriteMaterials(const XModelCommon& xmodel)
    {
        WriteUInt16(XModelBinHash::MATERIAL_COUNT, static_cast<uint16_t>(xmodel.m_materials.size()));

        size_t materialNum = 0u;
        for (const auto& material : xmodel.m_materials)
        {
            const auto colorMapPath = std::format("../images/{}.dds", material.colorMapName);

            WriteUInt16(XModelBinHash::MATERIAL, static_cast<uint16_t>(materialNum));
            WriteAlignedString(material.name);
            WriteAlignedString(material.materialTypeName);
            WriteAlignedString(colorMapPath);

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::COLOR));
            XModelBinWriterBase::Write(ClampFloatToUByte(material.color[0])); // R
            XModelBinWriterBase::Write(ClampFloatToUByte(material.color[1])); // G
            XModelBinWriterBase::Write(ClampFloatToUByte(material.color[2])); // B
            XModelBinWriterBase::Write(ClampFloatToUByte(material.color[3])); // A

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_TRANSPARENCY));
            XModelBinWriterBase::Write(material.transparency[0]);
            XModelBinWriterBase::Write(material.transparency[1]);
            XModelBinWriterBase::Write(material.transparency[2]);
            XModelBinWriterBase::Write(material.transparency[3]);

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_AMBIENT_COLOR));
            XModelBinWriterBase::Write(material.ambientColor[0]); // R
            XModelBinWriterBase::Write(material.ambientColor[1]); // G
            XModelBinWriterBase::Write(material.ambientColor[2]); // B
            XModelBinWriterBase::Write(material.ambientColor[3]); // A

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_INCANDESCENCE));
            XModelBinWriterBase::Write(material.incandescence[0]);
            XModelBinWriterBase::Write(material.incandescence[1]);
            XModelBinWriterBase::Write(material.incandescence[2]);
            XModelBinWriterBase::Write(material.incandescence[3]);

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_COEFFS));
            XModelBinWriterBase::Write(material.coeffs[0]);
            XModelBinWriterBase::Write(material.coeffs[1]);

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_GLOW));
            XModelBinWriterBase::Write(material.glow.x);
            XModelBinWriterBase::Write(material.glow.y);

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_REFRACTIVE));
            XModelBinWriterBase::Write(material.refractive.x);
            XModelBinWriterBase::Write(material.refractive.y);

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_SPECULAR_COLOR));
            XModelBinWriterBase::Write(material.specularColor[0]); // R
            XModelBinWriterBase::Write(material.specularColor[1]); // G
            XModelBinWriterBase::Write(material.specularColor[2]); // B
            XModelBinWriterBase::Write(material.specularColor[3]); // A

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_REFLECTIVE_COLOR));
            XModelBinWriterBase::Write(material.reflectiveColor[0]); // R
            XModelBinWriterBase::Write(material.reflectiveColor[1]); // G
            XModelBinWriterBase::Write(material.reflectiveColor[2]); // B
            XModelBinWriterBase::Write(material.reflectiveColor[3]); // A

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_REFLECTIVE));
            XModelBinWriterBase::Write(material.reflective.x);
            XModelBinWriterBase::Write(material.reflective.y);

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_BLINN));
            XModelBinWriterBase::Write(material.blinn[0]);
            XModelBinWriterBase::Write(material.blinn[1]);

            XModelBinWriterBase::Write(static_cast<uint32_t>(XModelBinHash::MATERIAL_PHONG));
            XModelBinWriterBase::Write(material.phong);

            materialNum++;
        }
    }

    std::ostream& m_stream;

public:
    XModelBinWriter7(std::ostream& stream, std::string gameName, std::string zoneName)
        : m_stream(stream),
          XModelBinWriterBase(std::move(gameName), std::move(zoneName))
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

        auto uncompressedSize = static_cast<uint32_t>(m_writer.str().size());
        auto estimatedCompressedFileSize = LZ4_compressBound(m_writer.str().size());
        const auto compressedBuffer = std::make_unique<char[]>(estimatedCompressedFileSize);
        auto actualCompressedFileSize =
            LZ4_compress_default(m_writer.str().c_str(), compressedBuffer.get(), m_writer.str().size(), estimatedCompressedFileSize);

        static constexpr char MAGIC[5] = {'*', 'L', 'Z', '4', '*'};
        m_stream.write(MAGIC, sizeof(MAGIC));
        m_stream.write(reinterpret_cast<char*>(&uncompressedSize), sizeof(uncompressedSize));
        m_stream.write(compressedBuffer.get(), actualCompressedFileSize);
    }
};

namespace xmodel_bin
{
    std::unique_ptr<XModelWriter> CreateWriterForVersion7(std::ostream& stream, std::string gameName, std::string zoneName)
    {
        return std::make_unique<XModelBinWriter7>(stream, std::move(gameName), std::move(zoneName));
    }
} // namespace xmodel_bin
