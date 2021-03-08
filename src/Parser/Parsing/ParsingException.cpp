#include "ParsingException.h"

#include <sstream>

ParsingException::ParsingException(const TokenPos position, std::string message)
    : m_pos(position),
      m_message(std::move(message))
{
    std::ostringstream str;
    str << position.m_filename.get() << " L" << m_pos.m_line << ':' << m_pos.m_column << ' ' << m_message;
    m_full_message = str.str();
}

TokenPos ParsingException::Position() const
{
    return m_pos;
}

const std::string& ParsingException::Message() const
{
    return m_message;
}

std::string ParsingException::FullMessage() const
{
    return m_full_message;
}

char const* ParsingException::what() const noexcept
{
    return m_full_message.c_str();
}
