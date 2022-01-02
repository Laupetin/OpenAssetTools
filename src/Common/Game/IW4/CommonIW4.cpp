#include "CommonIW4.h"

#include "Utils/Pack.h"

using namespace IW4;

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
