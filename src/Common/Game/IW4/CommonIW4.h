#pragma once

#include "IW4.h"

namespace IW4
{
    class Common
    {
    public:
        static constexpr uint32_t R_HashString(const char* string, const uint32_t hash)
        {
            const char* v2 = string; // edx@1
            char v3 = *string;       // cl@1
            uint32_t result = hash;

            for (; *v2; v3 = *v2)
            {
                ++v2;
                result = 33 * result ^ (v3 | 0x20);
            }
            return result;
        }

        static constexpr uint32_t R_HashString(const char* string)
        {
            return R_HashString(string, 0u);
        }

        static int StringTable_HashString(const char* str);

        static PackedTexCoords Vec2PackTexCoords(const float (&in)[2]);
        static PackedUnitVec Vec3PackUnitVec(const float (&in)[3]);
        static GfxColor Vec4PackGfxColor(const float (&in)[4]);
        static void Vec2UnpackTexCoords(const PackedTexCoords& in, float (&out)[2]);
        static void Vec3UnpackUnitVec(const PackedUnitVec& in, float (&out)[3]);
        static void Vec4UnpackGfxColor(const GfxColor& in, float (&out)[4]);
    };
} // namespace IW4
