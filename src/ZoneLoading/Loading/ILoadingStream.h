#pragma once

#include <cstdint>

class ILoadingStream
{
public:
    virtual ~ILoadingStream() = default;

    virtual size_t Load(void* buffer, size_t length) = 0;
    virtual int64_t Pos() = 0;
};