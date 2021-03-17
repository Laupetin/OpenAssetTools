#include "XChunkException.h"

XChunkException::XChunkException(std::string message)
    : m_message(std::move(message))
{
}

char const* XChunkException::what() const noexcept
{
    return m_message.c_str();
}

const std::string& XChunkException::Message() const
{
    return m_message;
}
