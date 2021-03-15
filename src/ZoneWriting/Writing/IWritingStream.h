#pragma once

#include <cstdint>
#include <cstddef>

class IWritingStream
{
public:
    IWritingStream() = default;
    virtual ~IWritingStream() = default;
    IWritingStream(const IWritingStream& other) = default;
    IWritingStream(IWritingStream&& other) noexcept = default;
    IWritingStream& operator=(const IWritingStream& other) = default;
    IWritingStream& operator=(IWritingStream&& other) noexcept = default;

    virtual void Write(const void* buffer, size_t length) = 0;
    virtual int64_t Pos() = 0;
};