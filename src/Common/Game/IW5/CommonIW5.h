#pragma once

#include "IW5.h"

namespace IW5
{
    class Common
    {
    public:
        static PackedTexCoords Vec2PackTexCoords(const vec2_t* in);
        static PackedUnitVec Vec3PackUnitVec(const vec3_t* in);
        static GfxColor Vec4PackGfxColor(const vec4_t* in);
        static void Vec2UnpackTexCoords(const PackedTexCoords& in, vec2_t* out);
        static void Vec3UnpackUnitVec(const PackedUnitVec& in, vec3_t* out);
        static void Vec4UnpackGfxColor(const GfxColor& in, vec4_t* out);
    };
}
