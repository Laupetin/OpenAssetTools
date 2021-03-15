#include "WritingException.h"

WritingException::WritingException(std::string message)
    : m_error_message(std::move(message))
{
}

char const* WritingException::what() const
{
    return m_error_message.c_str();
}

const std::string& WritingException::Message() const
{
    return m_error_message;
}
