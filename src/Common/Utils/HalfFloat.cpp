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

        result.u = ((half << 16) & 0x80000000) | (((((half << 14) & 0xFFFC000)
            - (~(half << 14) & 0x10000000)) ^ 0x80000000) >> 1);
        return result.f;
    }

    return 0.0f;
}

half_float_t HalfFloat::ToHalf(float f)
{
    return 0;
}
