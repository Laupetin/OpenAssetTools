#pragma once
#include <istream>

class FlacDecoder
{
public:
    FlacDecoder() = default;
    virtual ~FlacDecoder() = default;

    FlacDecoder(const FlacDecoder& other) = default;
    FlacDecoder(FlacDecoder&& other) noexcept = default;
    FlacDecoder& operator=(const FlacDecoder& other) = default;
    FlacDecoder& operator=(FlacDecoder&& other) noexcept = default;

    virtual bool Decode() = 0;
    virtual unsigned int GetFrameCount() = 0;
    virtual unsigned int GetFrameRate() = 0;
    virtual unsigned int GetNumChannels() = 0;

    static std::unique_ptr<FlacDecoder> Create(void* data, size_t length);
};
