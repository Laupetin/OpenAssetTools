#pragma once

#include <cstdint>

class Pack32
{
public:
    static uint32_t Vec2PackTexCoords(const float* in);
    static uint32_t Vec3PackUnitVec(const float* in);
    static void Vec2UnpackTexCoords(uint32_t in, float* out);
    static void Vec3UnpackUnitVec(uint32_t in, float* out);
};