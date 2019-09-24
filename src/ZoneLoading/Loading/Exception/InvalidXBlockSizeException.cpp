#include "InvalidXBlockSizeException.h"

InvalidXBlockSizeException::InvalidXBlockSizeException(const uint64_t size, const uint64_t max)
{
    m_size = size;
    m_max = max;
}

std::string InvalidXBlockSizeException::DetailedMessage()
{
    return "Zone uses more XBlock memory than allowed: " + std::to_string(m_size) + " (max is " + std::to_string(m_max) + ")";
}

char const* InvalidXBlockSizeException::what() const
{
    return "Zone has invalid block size";
}