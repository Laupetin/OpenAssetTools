#include "InvalidMagicException.h"
 
InvalidMagicException::InvalidMagicException(const char* expectedMagic)
{
    m_expected_magic = expectedMagic;
}

std::string InvalidMagicException::DetailedMessage()
{
    return "Expected magic '" + std::string(m_expected_magic) + "'";
}

char const* InvalidMagicException::what() const
{
    return "Encountered invalid magic when loading.";
}