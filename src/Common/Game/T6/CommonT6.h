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

        static PackedTexCoords Vec2PackTexCoords(const float (&in)[2]);
        static PackedUnitVec Vec3PackUnitVec(const float (&in)[3]);
        static GfxColor Vec4PackGfxColor(const float (&in)[4]);
        static void Vec2UnpackTexCoords(const PackedTexCoords& in, float (&out)[2]);
        static void Vec3UnpackUnitVec(const PackedUnitVec& in, float (&out)[3]);
        static void Vec4UnpackGfxColor(const GfxColor& in, float (&out)[4]);
    };
} // namespace T6
