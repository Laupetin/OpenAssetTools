#include "QuatInt16.h"

quat_int_16 QuatInt16::ToInt16(const float quat)
{
    return static_cast<quat_int_16>(quat * INT16_MAX);
}

float QuatInt16::ToFloat(const quat_int_16 quat)
{
    return static_cast<float>(quat) / static_cast<float>(INT16_MAX);
}
