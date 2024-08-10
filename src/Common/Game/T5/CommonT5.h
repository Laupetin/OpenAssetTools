#pragma once

#include "T5.h"

namespace T5
{
    class Common
    {
    public:
        static int Com_HashKey(const char* str, int maxLen);
        static int Com_HashString(const char* str);
        static int Com_HashString(const char* str, int len);

        static PackedTexCoords Vec2PackTexCoords(const float (&in)[2]);
        static PackedUnitVec Vec3PackUnitVec(const float (&in)[3]);
        static GfxColor Vec4PackGfxColor(const float (&in)[4]);
        static void Vec2UnpackTexCoords(const PackedTexCoords& in, float (&out)[2]);
        static void Vec3UnpackUnitVec(const PackedUnitVec& in, float (&out)[3]);
        static void Vec4UnpackGfxColor(const GfxColor& in, float (&out)[4]);
    };
} // namespace T5
