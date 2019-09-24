#pragma once

class ILoadingStream
{
public:
    virtual ~ILoadingStream() = default;

    virtual size_t Load(void* buffer, size_t length) = 0;
};