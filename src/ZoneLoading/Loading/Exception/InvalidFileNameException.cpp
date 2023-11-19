#include "InvalidFileNameException.h"

InvalidFileNameException::InvalidFileNameException(std::string& actualFileName, std::string& expectedFileName)
{
    m_actual_file_name = actualFileName;
    m_expected_file_name = expectedFileName;
}

std::string InvalidFileNameException::DetailedMessage()
{
    return "Name verification failed: The fastfile was created as '" + m_expected_file_name + "' but loaded as '" + m_actual_file_name + "'";
}

char const* InvalidFileNameException::what() const noexcept
{
    return "The filename when created and when loaded does not match";
}
