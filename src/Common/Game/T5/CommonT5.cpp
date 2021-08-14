#include "CommonT5.h"

#include <cctype>

#include "Utils/Pack.h"

using namespace T5;

int Common::Com_HashKey(const char* str, const int maxLen)
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

int Common::Com_HashString(const char* str)
{
    if (!str)
        return 0;

    auto result = 0x1505;
    auto offset = 0;
    while (str[offset])
    {
        const auto c = tolower(str[offset++]);
        result = c + 33 * result;
    }

    return result;
}

int Common::Com_HashString(const char* str, const int len)
{
    if (!str)
        return 0;

    int result = 0x1505;
    int offset = 0;
    while (str[offset])
    {
        if (len > 0 && offset >= len)
            break;

        const int c = tolower(str[offset++]);
        result = c + 33 * result;
    }

    return result;
}

PackedTexCoords Common::Vec2PackTexCoords(const vec2_t* in)
{
    return PackedTexCoords{Pack32::Vec2PackTexCoords(reinterpret_cast<const float*>(in))};
}

PackedUnitVec Common::Vec3PackUnitVec(const vec3_t* in)
{
    return PackedUnitVec{Pack32::Vec3PackUnitVec(reinterpret_cast<const float*>(in))};
}

GfxColor Common::Vec4PackGfxColor(const vec4_t* in)
{
    return GfxColor{Pack32::Vec4PackGfxColor(reinterpret_cast<const float*>(in))};
}

void Common::Vec2UnpackTexCoords(const PackedTexCoords& in, vec2_t* out)
{
    Pack32::Vec2UnpackTexCoordsVU(in.packed, reinterpret_cast<float*>(out));
}

void Common::Vec3UnpackUnitVec(const PackedUnitVec& in, vec3_t* out)
{
    Pack32::Vec3UnpackUnitVecScaleBased(in.packed, reinterpret_cast<float*>(out));
}

void Common::Vec4UnpackGfxColor(const GfxColor& in, vec4_t* out)
{
    Pack32::Vec4UnpackGfxColor(in.packed, reinterpret_cast<float*>(out));
}
