#include "GltfBufferView.h"

using namespace gltf;

BufferView::BufferView(const Buffer* buffer, const size_t offset, const size_t length, const size_t stride)
    : m_buffer(buffer),
      m_offset(offset),
      m_length(length),
      m_stride(stride)
{
}

bool BufferView::ReadElement(void* dest, const size_t elementIndex, const size_t elementSize) const
{
    const auto stride = std::max(elementSize, m_stride);
    const auto bufferViewOffset = elementIndex * stride;
    if (bufferViewOffset + elementSize > m_length)
        return false;

    const auto bufferOffset = m_offset + bufferViewOffset;

    return m_buffer->ReadData(dest, bufferOffset, elementSize);
}
