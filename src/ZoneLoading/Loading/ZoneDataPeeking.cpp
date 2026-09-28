#include "ZoneDataPeeking.h"

ZoneDataPeeking::ZoneDataPeeking(std::istream& stream)
    : m_stream(stream)
{
}

void ZoneDataPeeking::PeekWithStream(const std::function<void(std::istream& stream)>& cb) const
{
    const auto previousPos = m_stream.tellg();
    m_stream.seekg(0, std::ios::beg);

    cb(m_stream);

    m_stream.seekg(previousPos);
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
