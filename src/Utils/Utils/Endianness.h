#pragma once

#include <bit>
#include <concepts>
#include <cstdint>
#include <type_traits>

namespace endianness
{
    template<std::integral T> constexpr T ByteSwap(const T in)
    {
        using Unsigned_t = std::make_unsigned_t<T>;
        return static_cast<T>(std::byteswap(static_cast<Unsigned_t>(in)));
    }

    template<std::integral T> constexpr T ToLittleEndian(const T in)
    {
        static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big, "Mixed-endian platforms are not supported");

        if constexpr (std::endian::native == std::endian::little)
            return in;
        else
            return ByteSwap(in);
    }

    template<std::integral T> constexpr T FromLittleEndian(const T in)
    {
        return ToLittleEndian(in);
    }

    template<std::integral T> constexpr T ToBigEndian(const T in)
    {
        static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big, "Mixed-endian platforms are not supported");

        if constexpr (std::endian::native == std::endian::big)
            return in;
        else
            return ByteSwap(in);
    }

    template<std::integral T> constexpr T FromBigEndian(const T in)
    {
        return ToBigEndian(in);
    }
} // namespace endianness
