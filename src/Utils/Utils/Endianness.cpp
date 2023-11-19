#include "Endianness.h"

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)

namespace endianness
{
    constexpr uint16_t byteswap16u(const uint16_t in)
    {
        return static_cast<uint16_t>((in >> 8) | (in << 8));
    }

    constexpr int16_t byteswap16s(const int16_t in)
    {
        return static_cast<int16_t>(byteswap16u(static_cast<uint16_t>(in)));
    }

    constexpr uint32_t byteswap32u(const uint32_t in)
    {
        return static_cast<uint32_t>((in >> 24) | ((in >> 8) & 0x0000FF00ui32) | ((in << 8) & 0x00FF0000ui32) | (in << 24));
    }

    constexpr int32_t byteswap32s(const int32_t in)
    {
        return static_cast<int32_t>(byteswap32u(static_cast<uint32_t>(in)));
    }

    constexpr uint64_t byteswap64u(const uint64_t in)
    {
        return static_cast<uint32_t>((in >> 56) | ((in >> 40) & 0x000000000000FF00ui64) | ((in >> 24) & 0x0000000000FF0000ui64)
                                     | ((in >> 8) & 0x00000000FF000000ui64) | ((in << 8) & 0x000000FF00000000ui64) | ((in << 24) & 0x0000FF0000000000ui64)
                                     | ((in << 40) & 0x00FF000000000000ui64) | (in << 56));
    }

    constexpr int64_t byteswap64s(const int64_t in)
    {
        return static_cast<int64_t>(byteswap64u(static_cast<uint64_t>(in)));
    }
} // namespace endianness

#else

namespace endianness
{
    constexpr int16_t byteswap16s(const int16_t in)
    {
        return static_cast<int16_t>(__builtin_bswap16(static_cast<uint16_t>(in)));
    }

    constexpr uint16_t byteswap16u(const uint16_t in)
    {
        return __builtin_bswap16(in);
    }

    constexpr int32_t byteswap32s(const int32_t in)
    {
        return static_cast<int32_t>(__builtin_bswap32(static_cast<uint32_t>(in)));
    }

    constexpr uint32_t byteswap32u(const uint32_t in)
    {
        return __builtin_bswap32(in);
    }

    constexpr int64_t byteswap64s(const int64_t in)
    {
        return static_cast<int64_t>(__builtin_bswap64(static_cast<uint64_t>(in)));
    }

    constexpr uint64_t byteswap64u(const uint64_t in)
    {
        return __builtin_bswap64(in);
    }
} // namespace endianness

#endif

namespace endianness
{
#if HOST_ENDIANNESS == LITTLE_ENDIAN_ENDIANNESS

    int16_t ToBigEndian(const int16_t in)
    {
        return byteswap16s(in);
    }

    uint16_t ToBigEndian(const uint16_t in)
    {
        return byteswap16u(in);
    }

    int32_t ToBigEndian(const int32_t in)
    {
        return byteswap32s(in);
    }

    uint32_t ToBigEndian(const uint32_t in)
    {
        return byteswap32u(in);
    }

    int64_t ToBigEndian(const int64_t in)
    {
        return byteswap64s(in);
    }

    uint64_t ToBigEndian(const uint64_t in)
    {
        return byteswap64u(in);
    }

    int16_t FromBigEndian(const int16_t in)
    {
        return byteswap16s(in);
    }

    uint16_t FromBigEndian(const uint16_t in)
    {
        return byteswap16u(in);
    }

    int32_t FromBigEndian(const int32_t in)
    {
        return byteswap32s(in);
    }

    uint32_t FromBigEndian(const uint32_t in)
    {
        return byteswap32u(in);
    }

    int64_t FromBigEndian(const int64_t in)
    {
        return byteswap64s(in);
    }

    uint64_t FromBigEndian(const uint64_t in)
    {
        return byteswap64u(in);
    }

#else

    int16_t ToLittleEndian(const int16_t in)
    {
        return byteswap16s(in);
    }

    uint16_t ToLittleEndian(const uint16_t in)
    {
        return byteswap16u(in);
    }

    int32_t ToLittleEndian(const int32_t in)
    {
        return byteswap32s(in);
    }

    uint32_t ToLittleEndian(const uint32_t in)
    {
        return byteswap32u(in);
    }

    int64_t ToLittleEndian(const int64_t in)
    {
        return byteswap64s(in);
    }

    uint64_t ToLittleEndian(const uint64_t in)
    {
        return byteswap64u(in);
    }

    int16_t FromLittleEndian(const int16_t in)
    {
        return byteswap16s(in);
    }

    uint16_t FromLittleEndian(const uint16_t in)
    {
        return byteswap16u(in);
    }

    int32_t FromLittleEndian(const int32_t in)
    {
        return byteswap32s(in);
    }

    uint32_t FromLittleEndian(const uint32_t in)
    {
        return byteswap32u(in);
    }

    int64_t FromLittleEndian(const int64_t in)
    {
        return byteswap64s(in);
    }

    uint64_t FromLittleEndian(const uint64_t in)
    {
        return byteswap64u(in);
    }

#endif
} // namespace endianness
