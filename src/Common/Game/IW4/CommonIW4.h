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

        static PackedTexCoords Vec2PackTexCoords(const vec2_t* in);
        static PackedUnitVec Vec3PackUnitVec(const vec3_t* in);
        static GfxColor Vec4PackGfxColor(const vec4_t* in);
        static void Vec2UnpackTexCoords(const PackedTexCoords& in, vec2_t* out);
        static void Vec3UnpackUnitVec(const PackedUnitVec& in, vec3_t* out);
        static void Vec4UnpackGfxColor(const GfxColor& in, vec4_t* out);
    };
} // namespace IW4
