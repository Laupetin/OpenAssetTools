#pragma once

#include "IW5.h"
#include "Utils/Djb2.h"

#include <cctype>

namespace IW5
{
    class Common
    {
    public:
        static constexpr int StringTable_HashString(const char* str)
        {
            if (!str)
                return 0;

            // Lets do djb2 with 31 instead of 33 because why not
            // and leave out the starting value while we are at it
            uint32_t hash = 0;
            for (char c = *str; c; c = *++str)
                hash = hash * 31 + std::tolower(c);

            return static_cast<int>(hash);
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
} // namespace IW5
