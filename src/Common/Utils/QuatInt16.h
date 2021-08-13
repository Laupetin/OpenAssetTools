#pragma once

#include <cstdint>

typedef int16_t quat_int_16;

class QuatInt16
{
    QuatInt16() = default;

public:
    static quat_int_16 ToInt16(float quat);
    static float ToFloat(quat_int_16 quat);
};