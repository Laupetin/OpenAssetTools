#pragma once
#include <cstdint>

constexpr uint32_t MakeMagic32(const char ch0, const char ch1, const char ch2, const char ch3)
{
    return static_cast<uint32_t>(ch0)
        | static_cast<uint32_t>(ch1) << 8
        | static_cast<uint32_t>(ch2) << 16
        | static_cast<uint32_t>(ch3) << 24;
}
