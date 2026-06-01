#pragma once

#include <iostream>
#include <string>

namespace stream
{
    size_t Read(std::istream& stream, void* out, size_t outSize);

    template<typename T> T ReadValue(std::istream& stream)
    {
        T value{};
        stream.read(reinterpret_cast<char*>(&value), static_cast<std::streamsize>(sizeof(value)));

        return value;
    }

    template<typename T> void ReadValue(std::istream& stream, T& value)
    {
        stream.read(reinterpret_cast<char*>(&value), static_cast<std::streamsize>(sizeof(value)));
    }

    std::string ReadCString(std::istream& stream);

    void Write(std::ostream& stream, const void* in, size_t inSize);

    template<typename T> void WriteValue(std::ostream& stream, const T& value)
    {
        stream.write(reinterpret_cast<const char*>(&value), static_cast<std::streamsize>(sizeof(value)));
    }

    void WriteCString(std::ostream& stream, const std::string& value);
    void WriteCString(std::ostream& stream, const char* value);
} // namespace stream
