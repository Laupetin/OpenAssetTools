#include "InvalidVersionException.h"

InvalidVersionException::InvalidVersionException(const unsigned int expectedVersion, const unsigned int actualVersion)
{
    m_expected_version = expectedVersion;
    m_actual_version = actualVersion;
}

std::string InvalidVersionException::DetailedMessage()
{
    return "Expected version " + std::to_string(m_expected_version) + " but encountered version " + std::to_string(m_actual_version);
}

char const* InvalidVersionException::what() const noexcept
{
    return "Encountered invalid version when loading.";
}
