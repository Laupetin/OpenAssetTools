#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

enum class XBlockType : std::uint8_t
{
    BLOCK_TYPE_TEMP,
    BLOCK_TYPE_RUNTIME,
    BLOCK_TYPE_DELAY,
    BLOCK_TYPE_NORMAL
};

class XBlock
{
public:
    XBlock(std::string name, unsigned index, XBlockType type);

    void Alloc(size_t blockSize);

    std::string m_name;
    unsigned m_index;
    XBlockType m_type;

    std::unique_ptr<uint8_t[]> m_buffer;
    size_t m_buffer_size;
};
