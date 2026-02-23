#pragma once

#include "T5.h"
#include "Utils/Djb2.h"

namespace T5
{
    class Common
    {
    public:
        static constexpr int Com_HashKey(const char* str, const int maxLen)
        {
            if (str == nullptr)
                return 0;

            int hash = 0;
            for (int i = 0; i < maxLen; i++)
            {
                if (str[i] == '\0')
                    break;

                hash += str[i] * (0x77 + i);
            }

            return hash ^ ((hash ^ (hash >> 10)) >> 10);
        }

        static constexpr int Com_HashString(const char* str)
        {
            // Hashing aesthetics seem to be a thing
            if (!str)
                return 0;

            return static_cast<int>(djb2_lower(str));
        }

        static constexpr uint32_t R_HashString(const char* str, const uint32_t hash)
        {
            return djb2_xor_nocase(str, hash);
        }

        static constexpr uint32_t R_HashString(const char* string)
        {
            // Using djb2 with a 0 starting value makes a worse hash func apparently
            // but who am I to judge
            return R_HashString(string, 0u);
        }

        static PackedTexCoords Vec2PackTexCoords(const float (&in)[2]);
        static PackedUnitVec Vec3PackUnitVec(const float (&in)[3]);
        static GfxColor Vec4PackGfxColor(const float (&in)[4]);
        static void Vec2UnpackTexCoords(const PackedTexCoords& in, float (&out)[2]);
        static void Vec3UnpackUnitVec(const PackedUnitVec& in, float (&out)[3]);
        static void Vec4UnpackGfxColor(const GfxColor& in, float (&out)[4]);
    };
} // namespace T5
