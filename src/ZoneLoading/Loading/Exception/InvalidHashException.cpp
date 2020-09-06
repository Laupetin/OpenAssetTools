#include "InvalidHashException.h"

std::string InvalidHashException::DetailedMessage()
{
    return "Loaded fastfile has an invalid hash.";
}

char const* InvalidHashException::what() const
{
    return "Loaded fastfile has an invalid hash.";
}