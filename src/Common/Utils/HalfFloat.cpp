#include "HalfFloat.h"

float HalfFloat::ToFloat(const half_float_t half)
{
    if (half)
    {
        union
        {
            uint32_t u;
            float f;
        } result{};

        result.u = ((half << 16) & 0x80000000) | (((((half << 14) & 0xFFFC000) - (~(half << 14) & 0x10000000)) ^ 0x80000000) >> 1);
        return result.f;
    }

    return 0.0f;
}

half_float_t HalfFloat::ToHalf(const float f)
{
    half_float_t v3;
    int v6;

    union
    {
        uint32_t u;
        float f;
    } result{};

    result.f = f;

    if (static_cast<int>((2 * result.u) ^ 0x80000000) >> 14 < 0x3FFF)
        v6 = static_cast<int>((2 * result.u) ^ 0x80000000) >> 14;
    else
        v6 = 0x3FFF;

    if (v6 > -16384)
        v3 = static_cast<half_float_t>(v6);
    else
        v3 = 0xC000;

    return (v3 & 0x3FFFu) | ((result.u >> 16) & 0xC000u);
}
