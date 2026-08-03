#include "ZoneDataPeeking.h"

ZoneDataPeeking::ZoneDataPeeking(std::istream& stream)
    : m_stream(stream)
{
}

void ZoneDataPeeking::EnsureHasEnoughData(const size_t size)
{
    const auto currentSize = m_data.size();
    if (size > currentSize)
    {
        m_data.resize(size);
        m_stream.read(&m_data[currentSize], size - currentSize);
    }
}
