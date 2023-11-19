#include "IPakLoadException.h"

IPakLoadException::IPakLoadException(std::string message)
{
    m_message = std::move(message);
}

const std::string& IPakLoadException::DetailedMessage() const
{
    return m_message;
}

char const* IPakLoadException::what() const noexcept
{
    return "There was an error when trying to load an ipak file.";
}
