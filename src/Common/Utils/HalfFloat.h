#pragma once

#include <cstdint>

typedef uint16_t half_float_t;

class HalfFloat
{
    HalfFloat() = default;

public:
    static float ToFloat(half_float_t half);
    static half_float_t ToHalf(float f);
};
