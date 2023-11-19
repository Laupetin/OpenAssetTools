#include "TooManyAuthedGroupsException.h"

std::string TooManyAuthedGroupsException::DetailedMessage()
{
    return "Loaded fastfile has too many authed groups.";
}

char const* TooManyAuthedGroupsException::what() const noexcept
{
    return "Loaded fastfile has too many authed groups.";
}
