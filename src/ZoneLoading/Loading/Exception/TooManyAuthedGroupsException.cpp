#include "TooManyAuthedGroupsException.h"

std::string TooManyAuthedGroupsException::DetailedMessage()
{
    return "Loaded fastfile has too many authed groups.";
}

char const* TooManyAuthedGroupsException::what() const
{
    return "Loaded fastfile has too many authed groups.";
}