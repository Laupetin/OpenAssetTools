#include "VertexMerger.h"

#include <cmath>
#include <limits>

bool operator==(const VertexMergerPos& lhs, const VertexMergerPos& rhs)
{
    return std::fabs(lhs.x - rhs.x) < std::numeric_limits<float>::epsilon()
        && std::fabs(lhs.y - rhs.y) < std::numeric_limits<float>::epsilon()
        && std::fabs(lhs.z - rhs.z) < std::numeric_limits<float>::epsilon();
}

bool operator!=(const VertexMergerPos& lhs, VertexMergerPos& rhs)
{
    return !(lhs == rhs);
}

bool operator<(const VertexMergerPos& lhs, const VertexMergerPos& rhs)
{
    return std::tie(lhs.x, lhs.y, lhs.z) < std::tie(rhs.x, rhs.y, rhs.z);
}
/*
VertexMerger::VertexMerger()
    : m_current_vertex_index(0)
{
}

VertexMerger::VertexMerger(const size_t totalVertexCount)
    : m_current_vertex_index(0)
{
    m_position_lookup.reserve(totalVertexCount);
}

void VertexMerger::ProcessVertex(VertexPos pos)
{
    const auto mapEntry = m_vertex_index_map.find(pos);
    if (mapEntry == m_vertex_index_map.end())
    {
        m_position_lookup.push_back(m_current_vertex_index);
        m_unique_vertex_position_indices.push_back(m_current_vertex_index);
        m_vertex_index_map.emplace(std::make_pair(pos, m_current_vertex_index));
    }
    else
    {
        m_position_lookup.push_back(mapEntry->second);
    }

    m_current_vertex_index++;
}

size_t VertexMerger::GetUniqueVertexIndexForVertexIndex(const size_t vertexIndex)
{
    if (vertexIndex >= m_position_lookup.size())
        return 0;

    return m_position_lookup[vertexIndex];
}

const std::vector<size_t>& VertexMerger::GetUniqueVertexIndices() const
{
    return m_unique_vertex_position_indices;
}
*/