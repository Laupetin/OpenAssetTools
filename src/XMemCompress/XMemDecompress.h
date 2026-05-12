#pragma once

#include <cstdint>
#include <optional>

class XMemDecompressContext
{
public:
    XMemDecompressContext();
    ~XMemDecompressContext();
    XMemDecompressContext(const XMemDecompressContext& other) = delete;
    XMemDecompressContext(XMemDecompressContext&& other) noexcept;
    XMemDecompressContext& operator=(const XMemDecompressContext& other) = delete;
    XMemDecompressContext& operator=(XMemDecompressContext&& other) noexcept;

    void Reset() const;
    std::optional<size_t> Process(const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) const;

private:
    void* m_lzx_state;
};
