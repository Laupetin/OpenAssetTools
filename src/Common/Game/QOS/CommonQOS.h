#pragma once

#include "QOS.h"
#include "Utils/Djb2.h"

#include <cstdint>

namespace QOS
{
    class Common
    {
    public:
        static constexpr uint32_t R_HashString(const char* str, const uint32_t hash)
        {
            return djb2_xor_nocase(str, hash);
        }

        static constexpr uint32_t R_HashString(const char* string)
        {
            return R_HashString(string, 0u);
        }

        static PackedTexCoords Vec2PackTexCoords(const float (&in)[2]);
        static PackedUnitVec Vec3PackUnitVec(const float (&in)[3]);
        static GfxColor Vec4PackGfxColor(const float (&in)[4]);
        static void Vec2UnpackTexCoords(const PackedTexCoords& in, float (&out)[2]);
        static void Vec3UnpackUnitVec(const PackedUnitVec& in, float (&out)[3]);
        static void Vec4UnpackGfxColor(const GfxColor& in, float (&out)[4]);
    };
} // namespace QOS
