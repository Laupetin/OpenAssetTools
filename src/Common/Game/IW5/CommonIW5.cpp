#include "CommonIW5.h"

#include "Utils/Pack.h"

using namespace IW5;

int Common::StringTable_HashString(const char* str)
{
    if (!str)
        return 0;

    auto result = 0;
    auto offset = 0;
    while (str[offset])
    {
        const auto c = tolower(str[offset++]);
        result = c + 31 * result;
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
