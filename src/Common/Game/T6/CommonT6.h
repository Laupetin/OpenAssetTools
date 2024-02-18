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

        static constexpr uint32_t R_HashString(const char* str, uint32_t hash)
        {
            for (const auto* pos = str; *pos; pos++)
            {
                hash = 33 * hash ^ (*pos | 0x20);
            }

            return hash;
        }

        static constexpr uint32_t SND_HashName(const char* str)
        {
            if (!str || !*str)
                return 0;

            auto result = 0x1505;
            auto offset = 0u;

            while (str[offset])
            {
                const auto c = tolower(str[offset++]);
                result = c + 0x1003F * result;
            }

            if (!result)
                return 1;

            return result;
        }

        static PackedTexCoords Vec2PackTexCoords(const vec2_t* in);
        static PackedUnitVec Vec3PackUnitVec(const vec3_t* in);
        static GfxColor Vec4PackGfxColor(const vec4_t* in);
        static void Vec2UnpackTexCoords(const PackedTexCoords& in, vec2_t* out);
        static void Vec3UnpackUnitVec(const PackedUnitVec& in, vec3_t* out);
        static void Vec4UnpackGfxColor(const GfxColor& in, vec4_t* out);
    };
} // namespace T6
