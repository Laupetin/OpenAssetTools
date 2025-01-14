#include "InvalidFileNameException.h"

#include <format>

InvalidFileNameException::InvalidFileNameException(const std::string& actualFileName, const std::string& expectedFileName)
{
    m_actual_file_name = actualFileName;
    m_expected_file_name = expectedFileName;
}

std::string InvalidFileNameException::DetailedMessage()
{
    return std::format("Name verification failed: The fastfile was created as '{}' but loaded as '{}'", m_expected_file_name, m_actual_file_name);
}

char const* InvalidFileNameException::what() const noexcept
{
    return "The filename when created and when loaded does not match";
}
