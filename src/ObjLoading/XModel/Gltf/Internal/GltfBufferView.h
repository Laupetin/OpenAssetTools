#pragma once
#include "GltfBuffer.h"

namespace gltf
{
    class BufferView
    {
    public:
        BufferView(const Buffer* buffer, size_t offset, size_t length, size_t stride);

        bool ReadElement(void* dest, size_t elementIndex, size_t elementSize) const;

    private:
        const Buffer* m_buffer;
        size_t m_offset;
        size_t m_length;
        size_t m_stride;
    };
} // namespace gltf
