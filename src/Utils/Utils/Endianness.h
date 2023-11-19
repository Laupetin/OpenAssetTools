#pragma once

#include <cstdint>

#define LITTLE_ENDIAN_ENDIANNESS 1234
#define BIG_ENDIAN_ENDIANNESS 4321

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)

// Windows always considered Little Endian
#define HOST_ENDIANNESS LITTLE_ENDIAN_ENDIANNESS

#else

#include <endian.h>
#if _BYTE_ORDER == _LITTLE_ENDIAN
#define HOST_ENDIANNESS LITTLE_ENDIAN_ENDIANNESS
#else
#define HOST_ENDIANNESS BIG_ENDIAN_ENDIANNESS
#endif

#endif

namespace endianness
{
#if HOST_ENDIANNESS == LITTLE_ENDIAN_ENDIANNESS
    constexpr int16_t ToLittleEndian(const int16_t in)
    {
        return in;
    }

    constexpr uint16_t ToLittleEndian(const uint16_t in)
    {
        return in;
    }

    constexpr int32_t ToLittleEndian(const int32_t in)
    {
        return in;
    }

    constexpr uint32_t ToLittleEndian(const uint32_t in)
    {
        return in;
    }

    constexpr int64_t ToLittleEndian(const int64_t in)
    {
        return in;
    }

    constexpr uint64_t ToLittleEndian(const uint64_t in)
    {
        return in;
    }

    constexpr int16_t FromLittleEndian(const int16_t in)
    {
        return in;
    }

    constexpr uint16_t FromLittleEndian(const uint16_t in)
    {
        return in;
    }

    constexpr int32_t FromLittleEndian(const int32_t in)
    {
        return in;
    }

    constexpr uint32_t FromLittleEndian(const uint32_t in)
    {
        return in;
    }

    constexpr int64_t FromLittleEndian(const int64_t in)
    {
        return in;
    }

    constexpr uint64_t FromLittleEndian(const uint64_t in)
    {
        return in;
    }
#else
    int16_t ToLittleEndian(int16_t in);
    uint16_t ToLittleEndian(uint16_t in);
    int32_t ToLittleEndian(int32_t in);
    uint32_t ToLittleEndian(uint32_t in);
    int64_t ToLittleEndian(int64_t in);
    uint64_t ToLittleEndian(uint64_t in);
    int16_t FromLittleEndian(int16_t in);
    uint16_t FromLittleEndian(uint16_t in);
    int32_t FromLittleEndian(int32_t in);
    uint32_t FromLittleEndian(uint32_t in);
    int64_t FromLittleEndian(int64_t in);
    uint64_t FromLittleEndian(uint64_t in);
#endif

#if HOST_ENDIANNESS == BIG_ENDIAN_ENDIANNESS
    constexpr int16_t ToBigEndian(const int16_t in)
    {
        return in;
    }

    constexpr uint16_t ToBigEndian(const uint16_t in)
    {
        return in;
    }

    constexpr int32_t ToBigEndian(const int32_t in)
    {
        return in;
    }

    constexpr uint32_t ToBigEndian(const uint32_t in)
    {
        return in;
    }

    constexpr int64_t ToBigEndian(const int64_t in)
    {
        return in;
    }

    constexpr uint64_t ToBigEndian(const uint64_t in)
    {
        return in;
    }

    constexpr int16_t FromBigEndian(const int16_t in)
    {
        return in;
    }

    constexpr uint16_t FromBigEndian(const uint16_t in)
    {
        return in;
    }

    constexpr int32_t FromBigEndian(const int32_t in)
    {
        return in;
    }

    constexpr uint32_t FromBigEndian(const uint32_t in)
    {
        return in;
    }

    constexpr int64_t FromBigEndian(const int64_t in)
    {
        return in;
    }

    constexpr uint64_t FromBigEndian(const uint64_t in)
    {
        return in;
    }
#else
    int16_t ToBigEndian(int16_t in);
    uint16_t ToBigEndian(uint16_t in);
    int32_t ToBigEndian(int32_t in);
    uint32_t ToBigEndian(uint32_t in);
    int64_t ToBigEndian(int64_t in);
    uint64_t ToBigEndian(uint64_t in);
    int16_t FromBigEndian(int16_t in);
    uint16_t FromBigEndian(uint16_t in);
    int32_t FromBigEndian(int32_t in);
    uint32_t FromBigEndian(uint32_t in);
    int64_t FromBigEndian(int64_t in);
    uint64_t FromBigEndian(uint64_t in);
#endif
} // namespace endianness
