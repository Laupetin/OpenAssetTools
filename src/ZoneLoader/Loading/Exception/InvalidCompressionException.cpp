#include "InvalidCompressionException.h"

std::string InvalidCompressionException::DetailedMessage()
{
    return "Zone has invalid or unsupported compression. Inflate failed";
}

char const* InvalidCompressionException::what() const
{
    return "Zone has invalid or unsupported compression. Inflate failed";
}
