#pragma once

#include "T6.h"

namespace T6
{
    class Common
    {
    public:
        static int Com_HashKey(const char* str, int maxLen);
        static int Com_HashString(const char* str);
        static int Com_HashString(const char* str, int len);

        static PackedTexCoords Vec2PackTexCoords(const vec2_t* in);
        static PackedUnitVec Vec3PackUnitVec(const vec3_t* in);
        static GfxColor Vec4PackGfxColor(const vec4_t* in);
        static void Vec2UnpackTexCoords(const PackedTexCoords& in, vec2_t* out);
        static void Vec3UnpackUnitVec(const PackedUnitVec& in, vec3_t* out);
        static void Vec4UnpackGfxColor(const GfxColor& in, vec4_t* out);
    };
}