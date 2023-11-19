#include "InvalidSignatureException.h"

std::string InvalidSignatureException::DetailedMessage()
{
    return "Loaded fastfile has an invalid signature.";
}

char const* InvalidSignatureException::what() const noexcept
{
    return "Loaded fastfile has an invalid signature.";
}
