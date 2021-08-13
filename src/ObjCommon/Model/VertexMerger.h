#pragma once

#include "Utils/DistinctMapper.h"

struct VertexMergerPos
{
    float x;
    float y;
    float z;

    friend bool operator==(const VertexMergerPos& lhs, const VertexMergerPos& rhs);
    friend bool operator!=(const VertexMergerPos& lhs, const VertexMergerPos& rhs);
    friend bool operator<(const VertexMergerPos& lhs, const VertexMergerPos& rhs);
};

typedef DistinctMapper<VertexMergerPos> VertexMerger;

/*
#include <map>
#include <vector>

#include "Utils/ClassUtils.h"

class VertexMerger
{
public:
    struct VertexPos
    {
        float x;
        float y;
        float z;

        friend bool operator==(const VertexPos& lhs, const VertexPos& rhs);
        friend bool operator!=(const VertexPos& lhs, const VertexPos& rhs);
        friend bool operator<(const VertexPos& lhs, const VertexPos& rhs);
    };

    VertexMerger();
    explicit VertexMerger(size_t totalVertexCount);

    void ProcessVertex(VertexPos pos);
    size_t GetUniqueVertexIndexForVertexIndex(size_t vertexIndex);
    _NODISCARD const std::vector<size_t>& GetUniqueVertexIndices() const;

private:
    size_t m_current_vertex_index;
    std::vector<size_t> m_unique_vertex_position_indices;
    std::vector<size_t> m_position_lookup;
    std::map<VertexPos, int> m_vertex_index_map;
};
*/