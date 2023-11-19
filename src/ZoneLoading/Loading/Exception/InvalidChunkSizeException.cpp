#include "InvalidChunkSizeException.h"

InvalidChunkSizeException::InvalidChunkSizeException(const size_t size)
{
    m_size = size;
    m_max = 0;
}

InvalidChunkSizeException::InvalidChunkSizeException(const size_t size, const size_t max)
{
    m_size = size;
    m_max = max;
}

std::string InvalidChunkSizeException::DetailedMessage()
{
    if (m_max > 0)
    {
        return "Zone chunk size has a chunk size of " + std::to_string(m_size) + " which is larger than the maximum of " + std::to_string(m_max);
    }
    else
    {
        return "Zone chunk size has an invalid chunk size of " + std::to_string(m_size);
    }
}

char const* InvalidChunkSizeException::what() const noexcept
{
    return "Zone has invalid chunk size";
}
