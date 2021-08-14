#include "Pack.h"

#include <algorithm>
#include <cassert>

#include "HalfFloat.h"

union PackUtil32
{
    uint32_t u;
    int32_t i;
    float f;
    int8_t c[4];
    uint8_t uc[4];
};

uint32_t Pack32::Vec2PackTexCoords(const float* in)
{
    return static_cast<uint32_t>(HalfFloat::ToHalf(in[0])) << 16
        | HalfFloat::ToHalf(in[1]);
}

uint32_t Pack32::Vec3PackUnitVec(const float* in)
{
    // TODO
    return 0;
}

uint32_t Pack32::Vec4PackGfxColor(const float* in)
{
    return static_cast<uint8_t>(std::clamp(in[0], 0.0f, 1.0f) * 255.0f)
        | static_cast<uint8_t>(std::clamp(in[1], 0.0f, 1.0f) * 255.0f) << 8
        | static_cast<uint8_t>(std::clamp(in[2], 0.0f, 1.0f) * 255.0f) << 16
        | static_cast<uint8_t>(std::clamp(in[3], 0.0f, 1.0f) * 255.0f) << 24;
}

void Pack32::Vec2UnpackTexCoordsUV(const uint32_t in, float* out)
{
    const auto inHiDw = static_cast<half_float_t>((in >> 16) & UINT16_MAX);
    const auto inLoDw = static_cast<half_float_t>(in & UINT16_MAX);

    out[0] = HalfFloat::ToFloat(inLoDw);
    out[1] = HalfFloat::ToFloat(inHiDw);
}

void Pack32::Vec2UnpackTexCoordsVU(const uint32_t in, float* out)
{
    const auto inHiDw = static_cast<half_float_t>((in >> 16) & UINT16_MAX);
    const auto inLoDw = static_cast<half_float_t>(in & UINT16_MAX);

    out[0] = HalfFloat::ToFloat(inHiDw);
    out[1] = HalfFloat::ToFloat(inLoDw);
}

void Pack32::Vec3UnpackUnitVecScaleBased(const uint32_t in, float* out)
{
    assert(out != nullptr);

    PackUtil32 _in{in};
    const float decodeScale = (static_cast<float>(_in.uc[3]) - -192.0f) / 32385.0f;
    out[0] = (static_cast<float>(_in.uc[0]) + -127.0f) * decodeScale;
    out[1] = (static_cast<float>(_in.uc[1]) + -127.0f) * decodeScale;
    out[2] = (static_cast<float>(_in.uc[2]) + -127.0f) * decodeScale;
}

void Pack32::Vec3UnpackUnitVecThirdBased(const uint32_t in, float* out)
{
    PackUtil32 v0{ (in >> 0) & 0x3FF };
    PackUtil32 v1{ (in >> 10) & 0x3FF };
    PackUtil32 v2{ (in >> 20) & 0x3FF };

    v0.u = v0.u - 2 * (v0.u & 0x200) + 0x40400000;
    v1.u = v1.u - 2 * (v1.u & 0x200) + 0x40400000;
    v2.u = v2.u - 2 * (v2.u & 0x200) + 0x40400000;
    out[0] = (v0.f - 3.0f) * 8208.0312f;
    out[1] = (v1.f - 3.0f) * 8208.0312f;
    out[2] = (v2.f - 3.0f) * 8208.0312f;
}

void Pack32::Vec4UnpackGfxColor(uint32_t in, float* out)
{
    out[0] = static_cast<float>(in & UINT8_MAX) / 255.0f;
    out[1] = static_cast<float>((in >> 8) & UINT8_MAX) / 255.0f;
    out[2] = static_cast<float>((in >> 16) & UINT8_MAX) / 255.0f;
    out[3] = static_cast<float>((in >> 24) & UINT8_MAX) / 255.0f;
}
