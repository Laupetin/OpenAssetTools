#pragma once
#include <cstdint>

namespace tangent_space
{
    struct VertexData
    {
        const void* positionData;
        size_t positionDataStride;
        const void* normalData;
        size_t normalDataStride;
        const void* uvData;
        size_t uvDataStride;
        void* tangentData;
        size_t tangentDataStride;
        void* binormalData;
        size_t binormalDataStride;
        const uint16_t* triData;
    };

    void CalculateTangentSpace(const VertexData& vertexData, size_t triCount, size_t vertexCount);
} // namespace tangent_space
