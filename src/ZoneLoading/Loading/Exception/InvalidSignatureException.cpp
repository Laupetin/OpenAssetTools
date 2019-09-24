#include "InvalidSignatureException.h"

std::string InvalidSignatureException::DetailedMessage()
{
    return "Loaded fastfile has an invalid signature.";
}

char const* InvalidSignatureException::what() const
{
    return "Loaded fastfile has an invalid signature.";
}