#pragma once

#include <cstdint>

namespace pack32
{
    uint32_t Vec2PackTexCoordsUV(const float (&in)[2]);
    uint32_t Vec2PackTexCoordsVU(const float (&in)[2]);
    uint32_t Vec3PackUnitVecScaleBased(const float (&in)[3]);
    uint32_t Vec3PackUnitVecThirdBased(const float (&in)[3]);
    uint32_t Vec4PackGfxColor(const float (&in)[4]);

    void Vec2UnpackTexCoordsUV(uint32_t in, float (&out)[2]);
    void Vec2UnpackTexCoordsVU(uint32_t in, float (&out)[2]);
    void Vec3UnpackUnitVecScaleBased(uint32_t in, float (&out)[3]);
    void Vec3UnpackUnitVecThirdBased(uint32_t in, float (&out)[3]);
    void Vec4UnpackGfxColor(uint32_t in, float (&out)[4]);
}; // namespace pack32
