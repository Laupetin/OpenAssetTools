#include "StreamUtils.h"

#include <cstring>
#include <sstream>
#include <string>

namespace stream
{
    size_t Read(std::istream& stream, void* out, const size_t outSize)
    {
        stream.read(static_cast<char*>(out), static_cast<std::streamsize>(outSize));
        return static_cast<size_t>(stream.gcount());
    }

    std::string ReadCString(std::istream& stream)
    {
        std::string result;
        std::getline(stream, result, '\0');
        return result;
    }

    void Write(std::ostream& stream, const void* in, const size_t inSize)
    {
        stream.write(static_cast<const char*>(in), static_cast<std::streamsize>(inSize));
    }

    void WriteCString(std::ostream& stream, const std::string& value)
    {
        stream.write(value.c_str(), static_cast<std::streamsize>(value.size() + 1));
    }

    void WriteCString(std::ostream& stream, const char* value)
    {
        stream.write(value, static_cast<std::streamsize>(std::strlen(value) + 1));
    }
} // namespace stream
