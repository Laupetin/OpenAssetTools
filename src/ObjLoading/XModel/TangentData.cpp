#include "TangentData.h"

#include "Tangentspace.h"

void TangentData::CreateTangentData(const XModelCommon& common)
{
    if (common.m_vertices.empty())
        return;

    const auto vertexCount = common.m_vertices.size();
    m_tangents.resize(vertexCount);
    m_binormals.resize(vertexCount);

    auto triCount = 0u;
    for (const auto& object : common.m_objects)
        triCount += object.m_faces.size();

    std::vector<uint16_t> indices(triCount * 3u);
    auto triOffset = 0u;
    for (const auto& object : common.m_objects)
    {
        for (const auto& face : object.m_faces)
        {
            indices[triOffset++] = static_cast<uint16_t>(face.vertexIndex[0]);
            indices[triOffset++] = static_cast<uint16_t>(face.vertexIndex[1]);
            indices[triOffset++] = static_cast<uint16_t>(face.vertexIndex[2]);
        }
    }

    const auto& firstVertex = common.m_vertices[0];

    tangent_space::VertexData vertexData{
        firstVertex.coordinates,
        sizeof(XModelVertex),
        firstVertex.normal,
        sizeof(XModelVertex),
        firstVertex.uv,
        sizeof(XModelVertex),
        m_tangents.data(),
        sizeof(float) * 3,
        m_binormals.data(),
        sizeof(float) * 3,
        indices.data(),
    };

    tangent_space::CalculateTangentSpace(vertexData, triCount, vertexCount);
}
