#pragma once

#include <cstddef>
#include <cstdint>

class ILoadingStream
{
public:
    ILoadingStream() = default;
    virtual ~ILoadingStream() = default;
    ILoadingStream(const ILoadingStream& other) = default;
    ILoadingStream(ILoadingStream&& other) noexcept = default;
    ILoadingStream& operator=(const ILoadingStream& other) = default;
    ILoadingStream& operator=(ILoadingStream&& other) noexcept = default;

    virtual size_t Load(void* buffer, size_t length) = 0;
    virtual int64_t Pos() = 0;
};
