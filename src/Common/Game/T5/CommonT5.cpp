#include "CommonT5.h"

#include "Utils/Pack.h"

#include <cctype>

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

PackedTexCoords Common::Vec2PackTexCoords(const float (&in)[2])
{
    return PackedTexCoords{pack32::Vec2PackTexCoordsVU(in)};
}

PackedUnitVec Common::Vec3PackUnitVec(const float (&in)[3])
{
    return PackedUnitVec{pack32::Vec3PackUnitVecScaleBased(in)};
}

GfxColor Common::Vec4PackGfxColor(const float (&in)[4])
{
    return GfxColor{pack32::Vec4PackGfxColor(in)};
}

void Common::Vec2UnpackTexCoords(const PackedTexCoords& in, float (&out)[2])
{
    pack32::Vec2UnpackTexCoordsVU(in.packed, out);
}

void Common::Vec3UnpackUnitVec(const PackedUnitVec& in, float (&out)[3])
{
    pack32::Vec3UnpackUnitVecScaleBased(in.packed, out);
}

void Common::Vec4UnpackGfxColor(const GfxColor& in, float (&out)[4])
{
    pack32::Vec4UnpackGfxColor(in.packed, out);
}
