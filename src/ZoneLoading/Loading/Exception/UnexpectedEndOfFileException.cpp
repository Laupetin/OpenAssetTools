#include "UnexpectedEndOfFileException.h"

UnexpectedEndOfFileException::UnexpectedEndOfFileException() = default;

std::string UnexpectedEndOfFileException::DetailedMessage()
{
    return "Unexpected end of file";
}

char const* UnexpectedEndOfFileException::what() const noexcept
{
    return "Unexpected end of file";
}
